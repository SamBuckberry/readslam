#pragma once

#include "_sequence.h"
#include "_read.h"

namespace ReadSlam
{
	struct Assembly
	{
		Sequence forward;
		Sequence reverse;
		
		string name;
		int length;
		
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
			forward.init(name, true, sequence);
			reverse.init(name, false, sequence);
		}
		
		void build_index(int seed, bool bisulfite, bool usemap)
		{
			if (usemap)
			{
				forward.build_index_map(seed, bisulfite);
				reverse.build_index_map(seed, bisulfite);
			}
			else
			{
				forward.build_index(seed, bisulfite);
				reverse.build_index(seed, bisulfite);
			}
		}
		void close()
		{
			forward.clear();
			reverse.clear();
		}
	};
}
