#pragma once

#include "_stacker.h"
#include "_fasta.h"

/*
Make methyl-cytosine calls using the Benjamini-Hochberg false discovery rate correction 
technique. This works by calculating a table of p-values, then looping through the data
and counting the occurence of each p-value. These are then sorted by frequency, which is
assumed to be a normal distribution and based on the distribution, values are then 

ed. Finally, actual calls are made against the adjusted values.
*/
namespace ReadSlam
{
	struct Benjamini
	{
		static const int LIMIT = 60; //stack height limit (constrained by factorial)
		float cutoff;
		Stacker stacker;
		
		map<string, string> genome;
		
		struct pval {
			unsigned int n;
			unsigned int N;
			unsigned int count;
			double value;
		};
		vector<pval> pvals;
		
		//For sorting the pval table by counts
		static bool compare(const pval &a, const pval &b)
		{
			return a.value < b.value;
		}
		
		//Dump the probability table to screen
		void dump()
		{
			for (int i=0; i<LIMIT; ++i)
			{
				for (int j=0; j<LIMIT; ++j)
				{
					if (i <= j) cout << i << " " << j << " " << pvals[i*LIMIT+j].value << endl;
				}
			}
		}
		
		//Adjust the pvalues in the table using the BH approach
		void adjust()
		{
			vector<pval> temp;
			temp.resize(pvals.size());

			//Rank the pvalues lowest to highest
			sort(pvals.begin(), pvals.end(), compare);
			
			//Determine total count
			unsigned long total = 0;
			
			for (int i=0; i<pvals.size(); ++i)
			{
				if (pvals[i].n > 0 && pvals[i].n <= pvals[i].N)
				{
					total += pvals[i].count;
				}
			}
			
			//Adjust each pval. The rank for blocks is taken to be the midpoint
			unsigned long blockstart = 0;
			
			for (int i=0; i<pvals.size(); ++i)
			{
				int oldpos = (pvals[i].n * LIMIT) + pvals[i].N;
				
				if (pvals[i].n > 0 && pvals[i].n <= pvals[i].N)
				{
					//Set rank to be the midpoint of the block
					int rank = (blockstart + pvals[i].count) / 2;

					//Copy back into the original position
					temp[oldpos] = pvals[i];
					
					//Adjust the pvalue
					temp[oldpos].value = pvals[i].value * total / rank;

					//Advance to the next blockstart
					blockstart += pvals[i].count;
				}
				else
				{
					temp[oldpos].n = 0;
					temp[oldpos].N = 0;
					temp[oldpos].count = 0;
					temp[oldpos].value = 0;
				}
			}
			pvals = temp;
		}
		
		//Determine if a methyl-cytosine site is significant
		bool sig(int n, int N)
		{
			if (N > LIMIT)
			{
				n = (n * LIMIT) / N;
				N = LIMIT;
			}
			return pvals[(n*LIMIT)+N].value <= cutoff;
		}		
		
		//Determine the methylation context
		string get_context(string assembly, int position, bool forward)
		{
			if (forward)
			{
				if (position+2 >= genome[assembly].size()) return "CG";
				else if (genome[assembly][position+1] == 'G') return "CG";
				else if (genome[assembly][position+2] == 'G') return "CHG";
				else return "CHH";
			}
			else
			{
				if (position-2 < 0) return "CG";
				else if (genome[assembly][position-1] == 'C') return "CG";
				else if (genome[assembly][position-2] == 'C') return "CHG";
				else return "CHH";
			}
		}		
		
		//Make the mC calls
		void call(string ref, string context)
		{
			Stack* s;

			cout << "Making calls for context: " << context << endl;
	
			for (stacker.stacks_itr = stacker.stacks.begin(); stacker.stacks_itr != stacker.stacks.end(); ++stacker.stacks_itr)
			{
				if (stacker.stacks_itr->first == ref) continue;

				for (int i=0; i<stacker.stacks_itr->second.length; ++i)
				{
					s = &(stacker.stacks_itr->second.stacks[i]);
				
					if (s->fref == 'C' && s->fc > 0)
					{
						if (get_context(stacker.stacks_itr->first, i, true) == context)
						{
							s->fcall = sig(s->fc, s->ftotal) ? 'C' : (s->fcall == 'C' ? 'N' : s->fcall);
						}
					}
					else if (s->rref == 'C' && s->rc > 0)
					{
						if (get_context(stacker.stacks_itr->first, i, false) == context)
						{
							s->rcall = sig(s->rc, s->rtotal) ? 'C' : (s->rcall == 'C' ? 'N' : s->rcall);
						}
					}
				}
			}
		}
		
		//Count the number of instances of each n/N in the stacked reads
		void count(string ref, string context)
		{
			int len = pvals.size();
			
			for (int i=0; i<len; ++i)
			{
				pvals[i].count = 0;
			}
			
			Stack* s;

			cout << "Counting groups in context: " << context << endl;
	
			for (stacker.stacks_itr = stacker.stacks.begin(); stacker.stacks_itr != stacker.stacks.end(); ++stacker.stacks_itr)
			{
				if (stacker.stacks_itr->first == ref) continue;

				for (int i=0; i<stacker.stacks_itr->second.length; ++i)
				{
					int n = 0;
					int N = 0;
					
					s = &(stacker.stacks_itr->second.stacks[i]);
				
					if (s->fref == 'C')
					{
						if (get_context(stacker.stacks_itr->first, i, true) != context)
						{
							continue;
						}
						n = s->fc; 
						N = s->ftotal;
					}
					else if (s->rref == 'C')
					{
						if (get_context(stacker.stacks_itr->first, i, false) != context)
						{
							continue;
						}
						n = s->rc; 
						N = s->rtotal;
					}
					else
					{
						continue;
					}
					
					if (N > LIMIT)
					{ 
						n = (LIMIT*n)/N;
						N = LIMIT;
					}					
					pvals[(n*LIMIT)+N].count++;
				}
			}
		}		
		
		//Calculate probability of n in N given a known error rate (binomial probability distribution)
		//	        N!
		// p =  --------- * e^n * (1-e)^(N-n)	
	    //      n!*(N-n)!
		double probability(int n, int N, double e)
		{
			//Force a max value of N 60 (constrained by binomial)
			if (N > LIMIT)
			{
				n = (LIMIT*n)/N;
				N = LIMIT;
			}
			
			//Calculate the binomial coefficient using Pascal's Triangle (required to circumvent factorials)
			//Code from http://www.brpreiss.com/books/opus4/html/page467.html
			vector<long double> cs;
			cs.resize(N+1);
			cs[0] = 1;
			
			for (int i=1; i<=N; ++i)
			{
				cs[i] = 1;

				for (int j=i-1; j>0; --j)
				{
					cs[j] += cs[j-1];
				}
			}			
			return (double)(cs[n] * pow(e,n) * pow((1-e),(N-n)));
		}
		
		//Build a table of probabilities using the binomial distribution. 1/1,1/2,1/3,2/1,2/2/2/3 (n/N)
		void build_prob_table(double error)
		{
			cout << "Building prob table with error: " << error << endl;
			
			pvals.clear();
			pvals.resize(LIMIT * LIMIT);
		
			for (int n=0; n<LIMIT; ++n)
			{
				for (int N=0; N<LIMIT; ++N)
				{
					pval p;
					p.n = n;
					p.N = N;
					p.count = 0;
					p.value = probability(n,N,error);
					
					pvals[(n*LIMIT) + N] = p;
				}
			}
		}
		
		//Using a non-converted reference assembly, calculate the error rate in a specific mC context
		double get_error(string ref, string context)
		{
			stacker.stacks_itr = stacker.stacks.find(ref);
	
			if (stacker.stacks_itr == stacker.stacks.end())
			{
				cerr << "Unable to find assembly: " << ref << endl;
				exit(1);
			}
		
			Stack* s;

			long n = 0;
			long c = 0;
			
			for (int i=0, len=stacker.stacks[ref].length; i<len; ++i)
			{
				s = &(stacker.stacks[ref].stacks[i]);
				
				if (s->fref == 'C')
				{
					if (get_context(ref, i, true) == context)
					{
						n += s->ftotal;
						c += s->fc;
					}
				}
				else if (s->rref == 'C')
				{
					if (get_context(ref, i, false) == context)
					{
						n += s->rtotal;
						c += s->rc;
					}
				}
				else
				{
					continue;
				}
			}
			return (c < n) ? (double)c / (double)n : 1;
		}		
		
		//Load the genome from a reference file
		void load_genome(string infile)
		{
			cout << "Loading genome" << endl;
			FastA::Genome f;
			f.load(infile);
			genome.clear();
			
			for (int i=0; i<f.assemblies.size(); i++)
			{
				string name = f.assemblies[i].name;
				Strings::replace(name, "chr", "");
				Strings::replace(name, "_u", "");
				Strings::replace(name, " ", "");
				Strings::replace(name, ">", "");
				genome[name] = f.assemblies[i].sequence;
			}
			cout << "done" << endl;
		}
				
		//Make mC calls using the Benjamini-Hochberg FDR correction technique
		void benjamini(string genome, string reads, string stacks, string ref, float falsepos)
		{
			this->cutoff = falsepos;
			
			//Load the genome reference sequence
			load_genome(genome);
			
			//Build the stacks
			stacker.clear();
			stacker.load_genome(genome);
			stacker.stack(reads);
			stacker.call();
			
			//Make mC calls in each context
			vector<string> contexts;
			contexts.push_back("CG");
			contexts.push_back("CHG");
			contexts.push_back("CHH");
			
			for (int i=0; i<contexts.size(); ++i)
			{
				string context = contexts[i];
				
				cout << "Processing context: " << context << endl;
				cout << " - calculating error: " << flush;
				double error = get_error(ref,context);
				cout << ref << " " << context << " " << error << endl;

				cout << " - building p-table: " << flush;
				build_prob_table(error);
				cout << "done" << endl;

				cout << " - counting groups: " << flush;
				count(ref,context);
				cout << "done" << endl;
				
				dump();
				cout << " - adjusting cutoffs" << flush;
				adjust();
				cout << "done" << endl;
				dump();

				cout << " - making calls: " << flush;
				call(ref,context);
				cout << "done" << endl;
			}
			cout << "Saving stacks" << endl;
			stacker.split(stacks);
		}
	};
}
