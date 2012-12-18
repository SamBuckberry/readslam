#ifndef _READSLAM_HAMMER
#define _READSLAM_HAMMER

#include "_stacker.h"
#include "_fasta.h"

/* 
	Minor variant on hammer that allows entry of a fixed failed conversion rate rather than figuring it out at runtime.
	
	- determine from the control for a defined context
	- determine the conversion failure rate by some other means
	- pass that value in as a fixed parameter
	
 */

namespace ReadSlam
{
	struct HammerStatic
	{
		typedef vector<vector<double> > ProbTable;
	
		ProbTable probs;
		double stable;
		Stacker stacker;
		string ref;
		
		map<string, string> genome;
		
		void load_genome(string infile)
		{
			cout << "Loading genome" << endl;
			FastA::Genome f;
			f.load(infile);
			
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
		
		void hammer(string genome, string reads, string stacks, float failrate, float falsepos)
		{
			load_genome(genome);
			
			stacker.clear();
			stacker.load_genome(genome);
			stacker.stack(reads);
			stacker.call();
			
			vector<string> contexts;
			contexts.push_back("CG");
			contexts.push_back("CHG");
			contexts.push_back("CHH");
			
			for (int i=0; i<contexts.size(); ++i)
			{
				string context = contexts[i];
				
				cout << "Processing for context " << context << endl;
				cout << " - using fixed failure rate of: " << failrate << endl;
				double error = (double)failrate;

				cout << " - building p-table with error: " << error << endl;
				build_prob_table(error);

				cout << " - optimizing cutoff with false positive rate of: " << falsepos << endl;
				double cutoff = get_cutoff((double)falsepos, context);

				cout << " - updating calls" << endl;
				update(cutoff, context);
			}
			cout << "Saving stacks" << endl;
			stacker.split(stacks);
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

		//Determine the mC error rate from a reference assembly
		double get_error(string assembly, string context)
		{
			stacker.stacks_itr = stacker.stacks.find(assembly);
	
			if (stacker.stacks_itr == stacker.stacks.end())
			{
				cerr << "Unable to find assembly: " << assembly << endl;
				exit(1);
			}
		
			Stack* s;

			long n = 0;
			long c = 0;
	
			for (int i=0, len=stacker.stacks[assembly].length; i<len; ++i)
			{
				s = &(stacker.stacks[assembly].stacks[i]);
				
				if (s->fref == 'C')
				{
					if (get_context(assembly, i, true) == context)
					{
						n += s->ftotal;
						c += s->fc;
					}
				}
				else if (s->rref == 'C')
				{
					if (get_context(assembly, i, false) == context)
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
			//return 1 - (c < n ? (double)c / (double)n : 1);
			return (c < n) ? (double)c / (double)n : 1;
		}

		//Build a table of probabilities using the binomial distribution	
		void build_prob_table(double error)
		{
			cout << "Building prob table" << endl;
			
			int limit = 60;
			probs.clear();
			probs.resize(limit);
		
			for (int N=0; N<limit; ++N)
			{
				probs[N].resize(limit);
				
				//cout << N;
			
				for (int n=0; n<limit; ++n)
				{
					probs[N][n] = probability(n,N,error);
					//printf("\t%1.4f", probs[N][n]);
				}
				//cout << endl;
			}
		}
	
		//Set the % at which the binomial distribution stabilies (requred due to limitations of binomial)
		void set_stable(double cutoff)
		{
			double n = 0;
			double N = 0;
		
			int size = probs.size();
		
			vector<double> ps = probs.back();
		
			int first = -1;
		
			for (int i=0; i<ps.size(); ++i)
			{
				if (ps[i] <= cutoff)
				{
					stable = (double)i / (double)(ps.size());
					return;
				}
			}
			stable = 1.0;
		}
	
		//Calculate probability of n in N given a known error rate (binomial probability distribution)
		//	        N!
		// p =  --------- * e^n * (1-e)^(N-n)	
	    //      n!*(N-n)!
		double probability(int n, int N, double e)
		{
			//Force a max value of N 60 (constrained by binomial)
			if (N > 60)
			{
				n = (60*n)/N;
				N = 60;
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
			//return (double)(factorial(N) * pow(e,n) * pow((1-e),(N-n)))  /  (double)(factorial(n) * factorial(N-n));
		}
		
		//Lookup the probability table to determine if a call is significant
		bool sig(int n, int N, double cutoff)
		{
			if (n == 0 || N == 0) return false;
			
			if (N >= probs.size())
			{
				return (double)n / (double)N >= stable;
			}
			return probs[N][n] <= cutoff;
		}

		//Factorial function, not a good idea to try and calculate factorial > 100
		unsigned long factorial(int num)
		{
			unsigned long result = 1;

			while (num > 0)
			{
				result *= num;
				num--;
			}
			return result;
		}

		//Iteratively refine to determine the probability cutoff that satisfies the stated false positive level
		double get_cutoff(double falsepos, string context)
		{
			double coverage = 1.0;
			double last_coverage = 0.0;
			double cutoff = 0.0;
			int counter = 0;

			while (abs(coverage - last_coverage) > 0.0000001)
			{
				cout << " - iteration " << ++counter;

				if ((1.0 + falsepos) * coverage >= 1.0)
				{
					cutoff = 1.0;
				}
				else
				{
					cutoff = (falsepos * coverage) / (1.0 - coverage);
				}
				printf (", p-cutoff: %1.6f", cutoff);

				last_coverage = coverage;
				coverage = get_coverage(cutoff, context);

				printf (", coverage: %3.6f%%", 100*coverage);
				cout << endl;
			}
			return cutoff;
		}

		//Determine what the mC coverage (mC/C) would be given a specific probability cutoff
		double get_coverage(double cutoff, string context)
		{
			set_stable(cutoff);
		
			double mc;
			double c;

			Stack* s;

			for (stacker.stacks_itr = stacker.stacks.begin(); stacker.stacks_itr != stacker.stacks.end(); ++stacker.stacks_itr)
			{
				//if (stacker.stacks_itr->first == ref) continue;
				
				for (int i=0; i<stacker.stacks_itr->second.length; ++i)
				{
					s = &(stacker.stacks_itr->second.stacks[i]);
				
					if (s->fref == 'C')
					{
						if (get_context(stacker.stacks_itr->first, i, true) == context)
						{
							if (sig(s->fc, s->ftotal, cutoff)) mc++;
							c++;
						}
					}
					else if (s->rref == 'C')
					{
						if (get_context(stacker.stacks_itr->first, i, false) == context)
						{
							if (sig(s->rc, s->rtotal, cutoff)) mc++;
							c++;
						}
					}
				}
			}
			return mc/c;
		}

		//Make the mC calls
		void update(double cutoff, string context)
		{
			set_stable(cutoff);
			
			Stack* s;

			cout << "Launching final update with a cutoff of " << cutoff << endl;
	
			for (stacker.stacks_itr = stacker.stacks.begin(); stacker.stacks_itr != stacker.stacks.end(); ++stacker.stacks_itr)
			{
				//if (stacker.stacks_itr->first == ref) continue;

				for (int i=0; i<stacker.stacks_itr->second.length; ++i)
				{
					s = &(stacker.stacks_itr->second.stacks[i]);
				
					if (s->fref == 'C')
					{
						if (get_context(stacker.stacks_itr->first, i, true) == context)
						{
							if (sig(s->fc, s->ftotal, cutoff))
							{
								s->fcall = 'C';
							}
							else
							{
								if (s->fcall == 'C') s->fcall = 'N';
							}
						}
					}
					else if (s->rref == 'C')
					{
						if (get_context(stacker.stacks_itr->first, i, false) == context)
						{
							if (sig(s->rc, s->rtotal, cutoff))
							{
								s->rcall = 'C';
							}
							else
							{
								if (s->rcall == 'C') s->rcall = 'N';
							}
						}
					}
				}
			}
		}
	};
}
#endif