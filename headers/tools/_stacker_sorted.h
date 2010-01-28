#ifndef _READSLAM_STACKER
#define _READSLAM_STACKER

#include "../common/_common.h"
#include "../parsing/_slam.h"
#include "_fasta.h"

//A stacker that assumes loose sorting of reads in advance
namespace ReadSlam
{
	struct Stack
	{
		unsigned char fa,ft,fc,fg,fn;
		unsigned char ra,rt,rc,rg,rn;
		
		Stack()
		{
			fa = ft = fc = fg = fn = 0;
			ra = rt = rc = rg = rn = 0;
		}

		//Call the base for the stack
		void call()
		{
			vector<char> contenders;
			int max = 0;

			if (a > max) max = a;
			if (t > max) max = t;
			if (c > max) max = c;
			if (g > max) max = g;
			if (n > max) max = n;

			if (max == 0)
			{
				call = 'N';
				return;
			}
			contenders.clear();

			if (a == max) contenders.push_back(0);
			if (t == max) contenders.push_back(1);
			if (c == max) contenders.push_back(2);
			if (g == max) contenders.push_back(3);
			if (n == max) contenders.push_back(4);

			if (contenders.size() > 1)
			{
				random_shuffle(contenders.begin(), contenders.end());
			}
			switch(contenders[0])
			{
				case 0  : call = 'A'; break;
				case 1  : call = 'T'; break;
				case 2  : call = 'C'; break;
				case 3  : call = 'G'; break;
				default : call = 'N';
			}
		}
	};
	struct StackPair
	{
		string assembly;
		int position;
		Stack forward;
		Stack reverse;

		void call()
		{
			forward.call();
			reverse.call();
		}
		bool load(ifstream& in)
		{
			return (in 
				>> assembly
				>> position
				
				>> forward.ref
				>> forward.call
				>> forward.a
				>> forward.t
				>> forward.c
				>> forward.g
				>> forward.n
				
				>> reverse.ref
				>> reverse.call
				>> reverse.a
				>> reverse.t
				>> reverse.c
				>> reverse.g
				>> reverse.n
			);
		}
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';

			if (
				forward.a + forward.t + forward.c + forward.g + forward.n + 
				reverse.a + reverse.t + reverse.c + reverse.g + reverse.n == 0
			) return;

			out << assembly 
				<< tab << position
				<< tab << forward.ref
				<< tab << forward.call
				<< tab << forward.a
				<< tab << forward.t
				<< tab << forward.c
				<< tab << forward.g
				<< tab << forward.n

				<< tab << reverse.ref
				<< tab << reverse.call
				<< tab << reverse.a
				<< tab << reverse.t
				<< tab << reverse.c
				<< tab << reverse.g
				<< tab << reverse.n
			<< end;
		}
	};
	struct Stacker
	{
		 Stacker() { clear(); }
		~Stacker() { clear(); }

		void clear()
		{
			stacks.clear();
		}
		
		//Stack all the reads in a file. All reads in the file must be from the same assembly
		void stack_file(string infile)
		{
			ifstream in(infile.c_str());

			list<StackPair> stacks;
			list<StackPair>::itr;
			
			stacks.clear();
			
			BasicRead read;
			int progress = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << "\r" << flush;
				}
				if (read.assembly != assembly) return;
				if (read.position + read.sequence.size() >= stacks.size()) return;				
				add(read);
			}
			in.close();
		}		
				
		//Add a read to the stack
		void add(BasicRead& read)
		{
			if (read.assembly != assembly) return;
			if (read.position < 0) return;
			if (read.position + read.sequence.size() >= length) return;

			Stack* s;
			
			int readlen = read.sequence.size();
			bool forward = read.strand == "+";
			
			for (int i=0; i<readlen; ++i)
			{
				if (forward)
				{
					s = &(stacks[read.position+i].forward);
				}
				else
				{
					s = &(stacks[(read.position+readlen-1)-i].reverse);
				}
				if (s->a >= 255 || s->t >= 255 || s->c >= 255 || s->g >= 255 || s->n >= 255)
				{
					continue;
				}
				switch(read.sequence[i])
				{
					case 'A': s->a++; break;
					case 'T': s->t++; break;
					case 'C': s->c++; break;
					case 'G': s->g++; break;
					default : s->n++; break;
				}
			}
		}
	};
}
#endif