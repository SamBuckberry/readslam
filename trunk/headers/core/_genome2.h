#pragma once

#include "_index_vector.h"

namespace ReadSlam
{
	struct Assembly
	{
		string forward;
		string reverse;
		string name;
		int length;
		
		IndexVector indexF;
		IndexVector indexR;
	
		 Assembly() { clear(); }
		~Assembly() { clear(); }
		
		void clear()
		{
			forward.clear();
			reverse.clear();
			name.clear();
			length = 0;
		}
		void init(string name, string sequence)
		{
			this->name = name;
			this->length = sequence.size();
			forward = sequence;
			reverse = DNA::reverse_complement(sequence);
			
			index();
		}
		void index()
		{
			indexF.build(forward,12,false);
			indexR.build(reverse,12,false);
		}
	};
	
	struct Genome
	{
		map<string,Assembly> assemblies;
		
 		 Genome() { clear(); }
		~Genome() { clear(); }
		
		void clear()
		{
			assemblies.clear();
		}
		
		//Load from a FastA file
		void load(string infile)
		{
			ifstream in (infile.c_str());
			clear();

			while(in.peek() == '>')
			{
				string name;
				getline(in, name);
				name = name.substr(1);

				string line;
				ostringstream stream;

				while (getline(in,line))
				{
					stream << line;

					if (in.peek() == '>')
					{
						break;
					}
				}
				Assembly a;
				assemblies[name] = a;
				assemblies[name].init(name,stream.str());
			}
			in.close();
		}
	};
}
