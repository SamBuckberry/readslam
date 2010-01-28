namespace ReadSlam
{
	/*
	 * The caller is designed to gather significance stats for a single base
	 */
	struct BaseCaller
	{
		double error;
		typedef vector<vector<unsigned double> > ProbTable;
		
		map<string,Stacks>::iterator itr;
		string ref;
		
		void optimize(Stacks& stacks, char base, string ref, float falsepos)
		{
			itr = stacks.find(ref);
	
			if (itr == stacks.end())
			{
				cerr << "Unable to find reference assembly: " << ref << endl;
				exit(1);
			}
			this->ref = ref;
			this->base = base;
			this->error = get_error();
			
			cout << "Calculating error" << endl;

			double error = get_error();

			cout << "Using ref seq: " << ref << ", calculated error to be: " << error << endl;

			tabulate(error);

			cout << "Iterating to determine cutoff" << endl;
			double cutoff = get_cutoff((double)falsepos);

			cout << "Updating calls" << endl;
			update(cutoff);
		}
		
		//Set up the data structures
		void initialize()
		{
			map<char,map<char,double> > counts;
			map<char,map<char,double> > errors;
			
			vector<char> letters = {'A','T','C','G','N'};
			
			//Initialize elements
			for (int i=0; i<letters.size(); ++i)
			{
				map<char,double> temp1;
				map<char,double> temp2;

				counts[letters[i]] = temp1;
				errors[letters[i]] = temp2;
				
				for (int j=0; j<letters.size(); ++j)
				{
					counts[letters[i]][letters[j]] = 0;
					errors[letters[i]][letters[j]] = 0;
				}
			}			
		}
		
		//Calculate all error values
		void calculate_errors()
		{
			//Set the counts
			for (int i=0, len=itr->second.length; i<len; ++i)
			{
				Stack* s = &(itr->second.stacks[i]);
				
				counts[s->fref]['A'] += s->fa;
				counts[s->fref]['T'] += s->ft;
				counts[s->fref]['C'] += s->fc;
				counts[s->fref]['G'] += s->fg;
				counts[s->fref]['N'] += s->fn;

				counts[s->rref]['A'] += s->ra;
				counts[s->rref]['T'] += s->rt;
				counts[s->rref]['C'] += s->rc;
				counts[s->rref]['G'] += s->rg;
				counts[s->rref]['N'] += s->rn;
			}
			
			//Set the errors
			for (int i=0; i<letters.size(); ++i)
			{
				long total = 
					counts[letters[i]]['A'] +
					counts[letters[i]]['T'] +
					counts[letters[i]]['C'] +
					counts[letters[i]]['G'] +
					counts[letters[i]]['N']
				;
				for (int j=0; j<letters.size(); ++j)
				{
					errors[letters[i]][letters[j]] = counts[letters[i]][letters[j]] / total;
					probs[letters[i]][letters[j]] = tabulate(errors[letters[i]][letters[j]]);
				}
			}
		}
	
		//Build a table of probabilities using the binomial distribution	
		ProbTable tabulate(double error)
		{
			ProbTable probs;
			
			int limit = 60;
			probs.clear();
			probs.resize(limit);
		
			for (int N=0; N<limit; ++N)
			{
				probs[N].resize(limit);
				
				for (int n=0; n<limit; ++n)
				{
					probs[N][n] = probability(n,N,error);
				}
			}
			return probs;
		}
	
		//Calculate probability of n in N given a known error rate (binomial probability distribution)
		double probability(int n, int N, double e)
		{
			if (N > 60)
			{
				cerr << "Binomial probability calculation is limited to values of N <= 60" << endl;
				exit(1);
			}

			//Shortcut from http://www.brpreiss.com/books/opus4/html/page467.html
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
		
		//Lookup the probability table to determine if a call is significant
		bool sig(char ref, char base, int n, int N, double cutoff)
		{
			if (n == 0 || N == 0) return false;
			
			if (N >= probs.size())
			{
				n = ((probs.size()-1)*n)/N;
				N = probs.size()-1;
			}				
			return probs[ref][base][N][n] <= cutoff;
		}

		//Iteratively refine to determine the probability cutoff that satisfies the stated false positive level
		double get_cutoff(double falsepos)
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
				printf (", cutoff: %1.4f", cutoff);

				last_coverage = coverage;
				coverage = get_coverage(cutoff);

				printf (", coverage: %3.4f%%", 100*coverage);
				cout << endl;
			}
			return cutoff;
		}

		//Determine the base coverage (observed/expected) given a probability cutoff
		double get_coverage(double cutoff)
		{
			int total_n;
			int total_N;
			int n;

			Stack* s;

			int height = 0;
	
			for (itr = stacks.begin(); itr != stacks.end(); ++itr)
			{
				//if (itr->first == ref) continue;
				
				for (int i=0; i<itr->second.length; ++i)
				{
					s = &(itr->second.stacks[i]);
					
					if (s->fref != base && s->rref != base) continue;
				
					if (s->fref == base)
					{
						N = s->fa + s->ft + s->fc + s->fg + s->fn;

						switch(base)
						{
							case 'A': n = s->fa; break;
							case 'T': n = s->ft; break;
							case 'C': n = s->fc; break;
							case 'G': n = s->fg; break;
						}
						if (sig(n,N,cutoff))
						{
							total_n++;
						}
						total_N++;
					}
					else
					{
						N = s->ra + s->rt + s->rc + s->rg + s->rn;

						switch(base)
						{
							case 'A': n = s->ra; break;
							case 'T': n = s->rt; break;
							case 'C': n = s->rc; break;
							case 'G': n = s->rg; break;
						}
						if (sig(n,N,cutoff))
						{
							total_n++;
						}
						total_N++;
					}
				}
			}
			return (double)total_n / (double)total_N;
		}
	};
}
