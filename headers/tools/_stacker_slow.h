#ifndef _READSLAM_STACKER
#define _READSLAM_STACKER

#include "../common/_common.h"
#include "../parsing/_slam.h"
#include "_fasta.h"

namespace ReadSlam
{
	struct Stack
	{
		unsigned char a,t,c,g,n,ref,call;
		
		Stack()
		{
			a = t = c = g = n = 0;
			ref = call = 'N';
		}
	};
	struct StackPair
	{
		Stack forward;
		Stack reverse;
	};
	struct Stacks
	{
		string assembly;
		int length;
		vector<StackPair> stacks;
		
		 Stacks() { clear(); }
		~Stacks() { clear(); }

		void clear()
		{
			assembly.clear();
			length = 0;
			stacks.clear();
		}
		
		//Build the empty stack using a reference genome sequence
		void build(const string& name, const string& sequence)
		{
			this->assembly = name;
			this->length = sequence.size();

			stacks.clear();
			stacks.resize(length);

			for (int i=0; i<length; ++i)
			{
				stacks[i].forward.ref = sequence[i];
				
				switch(sequence[i])
				{
					case 'A' : stacks[i].reverse.ref = 'T'; break;
					case 'T' : stacks[i].reverse.ref = 'A'; break;
					case 'C' : stacks[i].reverse.ref = 'G'; break;
					case 'G' : stacks[i].reverse.ref = 'C'; break;
					default  : stacks[i].reverse.ref = 'N'; break;
				}
			}
		}
		
		//Add a read to the stack
		void add(BasicRead& read)
		{
			if (read.assembly != assembly) return;
			if (read.position < 0) return;
			if (read.position + read.sequence.size() >= length) return;
			return

			Stack* s;

			for (int i=0, len=read.sequence.size(); i<len; ++i)
			{
				if (read.strand == "+")
				{
					s = &(stacks[read.position+i].forward);
				}
				else
				{
					read.sequence = Strings::reverse(read.sequence);
					read.qualities = Strings::reverse(read.qualities);
					s = &(stacks[read.position+i].reverse);
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
		
		//Stack all the reads in a file. All reads in the file must be from the same assembly
		void stack_file(string infile)
		{
			ifstream in(infile.c_str());
			
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

			if (s->a > max) max = s->a;
			if (s->t > max) max = s->t;
			if (s->c > max) max = s->c;
			if (s->g > max) max = s->g;
			if (s->n > max) max = s->n;

			if (max == 0)
			{
				s->call = 'N';
				return;
			}
			contenders.clear();

			if (s->a == max) contenders.push_back(0);
			if (s->t == max) contenders.push_back(1);
			if (s->c == max) contenders.push_back(2);
			if (s->g == max) contenders.push_back(3);
			if (s->n == max) contenders.push_back(4);

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
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';

			Stack* f;
			Stack* r;

			for (int i=0; i<length; i++)
			{
				f = &(stacks[i].forward);
				r = &(stacks[i].reverse);

				if (f->a + f->t + f->c + f->g + f->n + r->a + r->t + r->c + r->g + r->n == 0) continue;

				out << assembly 
					<< tab << i 
					<< tab << f->ref
					<< tab << f->call
					<< tab << (int)(f->a)
					<< tab << (int)(f->t)
					<< tab << (int)(f->c)
					<< tab << (int)(f->g)
					<< tab << (int)(f->n)

					<< tab << r->ref
					<< tab << r->call
					<< tab << (int)(r->a)
					<< tab << (int)(r->t)
					<< tab << (int)(r->c)
					<< tab << (int)(r->g)
					<< tab << (int)(r->n)
				<< end;
			}
		}

		//Save a reduced file containing on the summary information
		void save_small(ofstream& out)
		{
			char end = '\n';

			for (int i=0; i<length; i++)
			{
				out << stacks[i].forward.call;
			}
			out << end;

			for (int i=0; i<length; i++)
			{
				out << stacks[i].reverse.call;
			}
			out << end;
		}
	};
	
	//Stacker
	struct Stacker
	{
		map<string,Stacks*> stacks;
		map<string,Stacks*>::iterator itr;
		
		//Expects the reference sequence to be a single FastA entry
		void stack(string refseq_file, string infile, string outfile)
		{
			cout << "Stacker initiated" << endl;
			cout << " - loading genome" << endl;
			
			FastA fasta;
			fasta.load(refseq_file);

			Stacks stack;

			for (int i=0; i<fasta.assemblies.size(); ++i)
			{
				string name = fasta.assemblies[i].name;
				Strings::replace(name, "chr", "");
				Strings::replace(name, "_u", "");
				Strings::replace(name, " ", "");
				Strings::replace(name, ">", "");
				
				string sequence = fasta.assemblies[i].sequence;
				cout << " - initializing stack for assembly " << name << endl;
				stacks[name] = new Stacks();
				stacks[name]->build(name,sequence);
			}
			
			//Stack the reads
			cout << " - stacking reads from file " << infile << endl;
			ifstream in(infile.c_str());
			
			BasicRead read;
			int progress = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " - " << progress << "\r" << flush;
				}
				itr = stacks.find(read.assembly);
				if (itr == stacks.end())
				{
					cout << read.assembly << " not found" << endl;
					continue;
				}
				stacks[read.assembly]->add(read);
			}
			in.close();			
			
			//Call the reads
			for (itr = stacks.begin(); itr != stacks.end(); ++itr)
			{
				cout << " - calling bases for assembly " << itr->first << endl;
				itr->second->call();
			}
			
			ofstream out (outfile.c_str());
			
			//Save the reads
			for (itr = stacks.begin(); itr != stacks.end(); ++itr)
			{
				cout << " - saving calls for assembly " << itr->first << endl;
				itr->second->save(out);
			}
			out.close();
		}
	};
}
#endif