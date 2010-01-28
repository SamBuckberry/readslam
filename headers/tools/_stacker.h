#ifndef _READSLAM_STACKER
#define _READSLAM_STACKER

#include "../common/_common.h"
#include "../parsing/_slam.h"
#include "_fasta.h"

namespace ReadSlam
{
	// struct Stack
	// {
	// 	char a,t,c,g,n;
	// 	char ref, call;
	// };
	struct Stack
	{
		unsigned short fa, ft, fc, fg, ftotal;
		unsigned short ra, rt, rc, rg, rtotal;
		char fref, fcall;
		char rref, rcall;
		
		Stack()
		{
			fa = ft = fc = fg = ftotal = 0;
			ra = rt = rc = rg = rtotal = 0;
			fref = fcall = 'N';
			rref = rcall = 'N';
		}
	};
	// struct StackPair
	// {
	// 	Stack forward;
	// 	Stack reverse;
	// };
	struct Stacks
	{
		string assembly;
		int length;
		vector<Stack> stacks;
		
		 Stacks() { clear(); }
		~Stacks() { clear(); }

		void clear()
		{
			assembly.clear();
			length = 0;
			stacks.clear();
		}
		
		//Initialize the stack using a reference sequence
		void initialize(const string& name, const string& sequence)
		{
			assembly = name;
			length = sequence.size();
			stacks.clear();
			stacks.resize(length);
			
			for (int i=0; i<length; ++i)
			{
				switch(sequence[i])
				{
					case 'A' : stacks[i].fref = 'A'; stacks[i].rref = 'T'; break;
					case 'T' : stacks[i].fref = 'T'; stacks[i].rref = 'A'; break;
					case 'C' : stacks[i].fref = 'C'; stacks[i].rref = 'G'; break;
					case 'G' : stacks[i].fref = 'G'; stacks[i].rref = 'C'; break;
					default  : stacks[i].fref = 'N'; stacks[i].rref = 'N'; break;
				}
			}
		}		
		
		//Add a read to the stack
		void add(BasicRead& read)
		{
			int readlen = read.sequence.size();
			
			if (read.assembly != assembly) return;
			if (read.position < 0) return;
			if (read.position + readlen >= length) return;

			Stack* s;

			if (read.strand == "+")
			{
				for (int i=0; i<readlen; ++i)
				{
					s = &(stacks[read.position+i]);

					switch(read.sequence[i])
					{
						case 'A': s->fa++; break;
						case 'T': s->ft++; break;
						case 'C': s->fc++; break;
						case 'G': s->fg++; break;
					}
					s->ftotal++;
				}
			}
			else
			{
				for (int i=0; i<readlen; ++i)
				{
					s = &(stacks[read.position+(readlen-1)-i]);

					switch(read.sequence[i])
					{
						case 'A': s->ra++; break;
						case 'T': s->rt++; break;
						case 'C': s->rc++; break;
						case 'G': s->rg++; break;
					}
					s->rtotal++;
				}
			}
		}
		
		//Remove a read from the stack
		void subtract(BasicRead& read)
		{
			int readlen = read.sequence.size();
			
			if (read.assembly != assembly) return;
			if (read.position < 0) return;
			if (read.position + readlen >= length) return;

			Stack* s;

			if (read.strand == "+")
			{
				for (int i=0; i<readlen; ++i)
				{
					s = &(stacks[read.position+i]);

					switch(read.sequence[i])
					{
						case 'A': if (s->fa > 0) s->fa--; break;
						case 'T': if (s->ft > 0) s->ft--; break;
						case 'C': if (s->fc > 0) s->fc--; break;
						case 'G': if (s->fg > 0) s->fg--; break;
					}
					s->ftotal--;
				}
			}
			else
			{
				for (int i=0; i<readlen; ++i)
				{
					s = &(stacks[read.position+(readlen-1)-i]);

					switch(read.sequence[i])
					{
						case 'A': if (s->ra > 0) s->ra--; break;
						case 'T': if (s->rt > 0) s->rt--; break;
						case 'C': if (s->rc > 0) s->rc--; break;
						case 'G': if (s->rg > 0) s->rg--; break;
					}
					s->rtotal--;
				}
			}
		}
		
		void call()
		{
			for (int i=0; i<length; ++i)
			{
				call_stack(stacks[i]);
			}
		}
		
		void call_stack(Stack& s)
		{
			vector<char> contenders;
			int max = 0;
			int n = 0;

			//Forward call
			contenders.clear();
			max = 0;
			
			if (s.fa > max) max = s.fa;
			if (s.ft > max) max = s.ft;
			if (s.fc > max) max = s.fc;
			if (s.fg > max) max = s.fg;

			n = s.ftotal - (s.fa + s.ft + s.fc + s.fg);
			if (n > max) max = n;
			
			if (max > 0)
			{
				if (s.fa == max) contenders.push_back(0);
				if (s.ft == max) contenders.push_back(1);
				if (s.fc == max) contenders.push_back(2);
				if (s.fg == max) contenders.push_back(3);
				if (n    == max) contenders.push_back(4);

				if (contenders.size() > 1)
				{
					random_shuffle(contenders.begin(), contenders.end());
				}
				switch(contenders[0])
				{
					case 0  : s.fcall = 'A'; break;
					case 1  : s.fcall = 'T'; break;
					case 2  : s.fcall = 'C'; break;
					case 3  : s.fcall = 'G'; break;
					default : s.fcall = 'N';
				}
			}
			else 
			{
				s.fcall = 'N';
			}
			
			//Reverse call
			contenders.clear();
			max = 0;
			
			if (s.ra > max) max = s.ra;
			if (s.rt > max) max = s.rt;
			if (s.rc > max) max = s.rc;
			if (s.rg > max) max = s.rg;

			n = s.rtotal - (s.ra + s.rt + s.rc + s.rg);
			if (n > max) max = n;
			
			if (max > 0)
			{
				if (s.ra == max) contenders.push_back(0);
				if (s.rt == max) contenders.push_back(1);
				if (s.rc == max) contenders.push_back(2);
				if (s.rg == max) contenders.push_back(3);
				if (n    == max) contenders.push_back(4);

				if (contenders.size() > 1)
				{
					random_shuffle(contenders.begin(), contenders.end());
				}
				switch(contenders[0])
				{
					case 0  : s.rcall = 'A'; break;
					case 1  : s.rcall = 'T'; break;
					case 2  : s.rcall = 'C'; break;
					case 3  : s.rcall = 'G'; break;
					default : s.rcall = 'N';
				}
			}
			else 
			{
				s.rcall = 'N';
			}
		}
		
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';

			Stack* s;
						
			for (int i=0; i<length; i++)
			{
				s = &(stacks[i]);

				if (s->ftotal + s->rtotal == 0)
				{
					continue;
				}
				out << assembly 
					<< tab << i 
					<< tab << s->fref
					<< tab << s->fcall
					<< tab << s->fa
					<< tab << s->ft
					<< tab << s->fc
					<< tab << s->fg
					<< tab << s->ftotal

					<< tab << s->rref
					<< tab << s->rcall
					<< tab << s->ra
					<< tab << s->rt
					<< tab << s->rc
					<< tab << s->rg
					<< tab << s->rtotal
				<< end;
			}
		}
	};
	
	//Stacker
	struct Stacker
	{
		map<string,Stacks> stacks;
		map<string,Stacks>::iterator stacks_itr;
		bool ready;
		
		 Stacker() { clear(); }
		~Stacker() { clear(); }
		
		void clear()
		{
			stacks.clear();
			ready = false;
		}

		//Initialize stacks using a reference genome (FastA file)
		void load_genome(string ref)
		{
			clear();
			
			cout << "Loading genome" << endl;
			FastA::Genome genome;
			genome.load(ref);

			for (int i=0; i<genome.assemblies.size(); ++i)
			{
				FastA::Assembly* a = &(genome.assemblies[i]);
				
				Strings::replace(a->name, "chr", "");
				Strings::replace(a->name, "_u", "");
				Strings::replace(a->name, " ", "");
				Strings::replace(a->name, ">", "");
								
				cout << " - initializing stack for assembly " << a->name << " (" << a->length << ")" << endl;
				
				Stacks s;
				stacks[a->name] = s;
				stacks[a->name].initialize(a->name,a->sequence);
			}
			genome.clear();
			ready = true;
		}
		
		//Stack a file of reads
		void stack(string infile)
		{
			//Stack the reads
			cout << "Stacking reads from file " << infile << endl;
			ifstream in(infile.c_str());
			
			BasicRead read;
			int progress = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " - " << progress << "\r" << flush;
				}
				stacks_itr = stacks.find(read.assembly);
				
				if (stacks_itr == stacks.end())
				{
					continue;
				}
				stacks[read.assembly].add(read);
			}
			in.close();
			cout << endl;
		}
		
		void save(string outfile)
		{
			cout << "Saving stacks to file " << outfile << endl;
			ofstream out (outfile.c_str());
			
			for (stacks_itr = stacks.begin(); stacks_itr != stacks.end(); ++stacks_itr)
			{
				cout << " - saving stacks for assembly " << stacks_itr->first << endl;
				stacks_itr->second.save(out);
			}
			out.close();
		}
		
		void call()
		{
			cout << "Making base calls" << endl;
			
			for (stacks_itr = stacks.begin(); stacks_itr != stacks.end(); ++stacks_itr)
			{
				cout << " - calling on assembly " << stacks_itr->first << endl;
				stacks_itr->second.call();
			}
		}
		
		//Load stacks into memory (requires that genome be loaded first)
		void load(string infile)
		{
			if (!ready)
			{
				cerr << "Error: genome file not loaded yet, unable to load stacks" << endl;
				exit(1);
			}
			cout << "Loading stacks from file " << infile << endl;
			ifstream in (infile.c_str());
			
			string assembly;
			int position;
			int errors = 0;
			int progress = 0;
			
			Stack* s;
			
			while (in >> assembly >> position)
			{
				if (++progress % 100000 == 0)
				{
					cout << ' '  << progress << '\r' << flush;
				}
				stacks_itr = stacks.find(assembly);
				
				if (stacks_itr == stacks.end())
				{
					++errors;
					continue;
				}
				if (position < 0 || position >= stacks[assembly].length)
				{
					++errors;
					continue;
				}
				
				s = &(stacks[assembly].stacks[position]);
				
				in 
					>> s->fref 
					>> s->fcall 
					>> s->fa 
					>> s->ft 
					>> s->fc 
					>> s->fg 
					>> s->ftotal
				
					>> s->rref 
					>> s->rcall 
					>> s->ra 
					>> s->rt 
					>> s->rc 
					>> s->rg 
					>> s->rtotal
				;
			}
			in.close();
		}
		
		//Split a loads set of stacks into many outfiles
		void split(string outfile)
		{
			ofstream out;

			for (stacks_itr = stacks.begin(); stacks_itr != stacks.end(); ++stacks_itr)
			{
				out.open((outfile + "." + stacks_itr->first).c_str());
				cout << " - saving stacks for assembly " << stacks_itr->first << endl;
				stacks_itr->second.save(out);
				out.close();
			}
		}
	};
}
#endif