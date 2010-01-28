#ifndef _READSLAM_STACKER
#define _READSLAM_STACKER

#include "../common/_common.h"
#include "../core/_read.h"

namespace ReadSlam
{
	struct Stack
	{
		unsigned int na, nt, nc, ng, nn;
		unsigned int qa, qt, qc, qg, qn;
		unsigned int max, sum;
		char ref, call;
		
		Stack()
		{
			na = nt = nc = ng = nn = 0;
			qa = qt = qc = qg = qn = 0;
			max = sum = 0;
			ref = call = 'N';
		}
	};
	struct StackPair
	{
		Stack forward;
		Stack reverse;
	};
	struct Stacker
	{
		string assembly;
		int  length;
		bool bisulfite;
	
		vector<StackPair> stacks;
		string fseq;
		string rseq;
		
		 Stacker() { clear(); }
		~Stacker() { clear(); }
		
		void clear()
		{
			assembly.clear();
			length = 0;
			bisulfite = false;
			stacks.clear();
			fseq.clear();
			rseq.clear();
		}
		
		//WARNING: sanity check this for reverse complementation of crick strand
		void init(string name, string sequence, bool bisulfite)
		{
			this->assembly = name;
			this->fseq = sequence;
			this->rseq = DNA::complement(sequence);
			this->length = sequence.size();
			this->bisulfite = bisulfite;
			
			stacks.clear();
			stacks.resize(length);
			
			for (int i=0; i<length; ++i)
			{
				stacks[i].forward.ref = fseq[i];
				stacks[i].reverse.ref = rseq[i];
			}
		}
		
		//Stack all the reads in a file. All reads in the file must be from the same assembly
		void stack_file(string infile, string outfile)
		{
			clear();
			
			ifstream in(infile.c_str());
			
			Read read;
			int progress = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << "\r" << flush;
				}
				if (assembly == "") assembly = read.assembly;
				if (read.assembly != assembly)
				{
					cerr << "All reads in the file must be from the same assembly" << endl;
					exit(1);
				}
				if (read.position + read.length >= stacks.size())
				{
					stacks.resize(read.position + read.length + 1);
					length = stacks.size();
				}				
				add(read);
			}
			in.close();
			
			call();
			save(outfile);
		}

		void add(Read& read)
		{
			if (read.assembly != assembly) return;
			if (read.position < 0) return;
			if (read.position + read.length >= length) return;
			
			Stack* s;

			for (int i=0; i<read.length; ++i)
			{
				if (read.forward)
				{
					s = &(stacks[read.position+i].forward);
				}
				else
				{
					read.sequence = Strings::reverse(read.sequence);
					read.qualities = Strings::reverse(read.qualities);
					s = &(stacks[read.position+i].reverse);
				}
				switch(read.sequence[i])
				{
					case 'A': s->na++; s->qa += read.qualities[i]; break;
					case 'T': s->nt++; s->qt += read.qualities[i]; break;
					case 'C': s->nc++; s->qc += read.qualities[i]; break;
					case 'G': s->ng++; s->qg += read.qualities[i]; break;
					default : s->nn++; s->qn += read.qualities[i]; break;
				}
			}
		}
		
		//Make base calls based on the final stacks
		void call()
		{
			for (int i=0, limit=stacks.size(); i<limit; ++i)
			{
				call_stack(&(stacks[i].forward));
				call_stack(&(stacks[i].reverse));
			}
		}
		
		void call_stack(Stack* s)
		{
			vector<char> contenders;
			int max = 0;
			
			if (s->qa > max) max = s->qa;
			if (s->qt > max) max = s->qt;
			if (s->qc > max) max = s->qc;
			if (s->qg > max) max = s->qg;
			if (s->qn > max) max = s->qn;
			
			if (max == 0)
			{
				s->max = 0;
				s->sum = 0;
				s->call = 'N';
				return;
			}
			s->max = max;
			s->sum = s->qa + s->qt + s->qc + s->qg + s->qn;
			
			contenders.clear();
			
			if (s->qa == max) contenders.push_back(0);
			if (s->qt == max) contenders.push_back(1);
			if (s->qc == max) contenders.push_back(2);
			if (s->qg == max) contenders.push_back(3);
			if (s->qn == max) contenders.push_back(4);

			if (contenders.size() > 1)
			{
				random_shuffle(contenders.begin(), contenders.end());
			}
			switch(contenders[0])
			{
				case 0  : s->call = 'A'; break;
				case 1  : s->call = 'T'; break;
				case 2  : s->call = 'C'; break;
				case 3  : s->call = 'G'; break;
				default : s->call = 'N';
			}
		}
		
		//Save stacks to file
		void save(string outfile)
		{
			ofstream out (outfile.c_str());

			char tab = '\t';
			
			Stack* f;
			Stack* r;

			for (int i=0; i<length; i++)
			{
				f = &(stacks[i].forward);
				r = &(stacks[i].reverse);
				
				if (f->sum == 0 && r->sum == 0) continue;
				
				out << i 
					<< tab << f->ref << tab << f->call
					<< tab << f->na << tab << f->qa
					<< tab << f->nt << tab << f->qt
					<< tab << f->nc << tab << f->qc
					<< tab << f->ng << tab << f->qg
					<< tab << f->nn << tab << f->qn
					
					<< tab << r->ref << tab << r->call
					<< tab << r->na << tab << r->qa
					<< tab << r->nt << tab << r->qt
					<< tab << r->nc << tab << r->qc
					<< tab << r->ng << tab << r->qg
					<< tab << r->nn << tab << r->qn
				<< endl;
			}
			out.close();
		}
		
		//Save a reduced file containing on the summary information
		void save_small(string outfile)
		{
			ofstream out (outfile.c_str());

			char tab = '\t';
			
			Stack* f;
			Stack* r;

			for (int i=0; i<length; i++)
			{
				f = &(stacks[i].forward);
				r = &(stacks[i].reverse);
				
				if (f->sum == 0 && r->sum == 0) continue;
				
				out << i 
					<< tab << f->ref
					<< tab << f->call
					<< tab << f->max
					<< tab << f->sum
				<< endl;
			}
			out.close();
		}
	};
}
#endif