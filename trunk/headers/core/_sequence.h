#pragma once

#include "_dna.h"
#include <map>

/**
 * Represents a single sequence and an index for it
 */
namespace ReadSlam
{
	struct Sequence
	{
		string name;
		string sequence;
		int    length;
		bool   forward;
		bool   bisulfite;
	
		vector<int> index_random;
		vector<int> index_sorted;
		vector<int> index_counts;
		vector<int> index_offsets;

		//The other indexing system
		map<int, list<int> > index_map;
		map<int, list<int> >::iterator index_iterator;
		
		 Sequence() { clear(); }
		~Sequence() { clear(); }
		
		void clear()
		{
			name.clear();
			sequence.clear();
			index_random.clear();
			index_sorted.clear();
			index_counts.clear();
			index_offsets.clear();
			index_map.clear();

			length    = 0;
			forward   = true;
			bisulfite = false;
		}
		
		void init(string name, bool forward, string sequence)
		{
			this->name = name;
			this->forward = forward;
			this->length = sequence.size();
			this->sequence = forward ? sequence : DNA::reverse_complement(sequence);
		}
		
		void build_index(int seed, bool bisulfite)
		{
			this->bisulfite = bisulfite;
			
			int base = 4;
			int max = (int)pow((double)base, (double)seed);
		
			//Initialize index
			index_random.resize(length,-1);
			index_sorted.resize(length,-1);
			index_counts.resize(max,0);
			index_offsets.resize(max,0);
		
			//Temporary store used when sorting index
			vector<int> index_temp;
			index_temp.resize(max,0);
			
			//Populate randomly ordered index
			DNA::seq2indices(sequence, index_random, seed, bisulfite);
			
			//Populate counts
			for (int i=0; i<length; ++i)
			{
				int index = index_random[i];
				
				if (index != -1)
				{
					index_counts[index]++;
				}
			}
			
			//Populate offsets
			for (int i=0, offset=0; i<max; ++i)
			{
				index_offsets[i] = offset;
				offset += index_counts[i];
			}
			
			//Populate ordered index
			for (int i=0; i<length; ++i)
			{
				if (i%100000 == 0)
				{
					cout << (forward ? "+" : "-") << i <<  "            \r" << flush;
				}				
				int index = index_random[i];
		
				if (index != -1)
				{
					int offset = index_offsets[index] + index_temp[index];
					index_sorted[offset] = i;
					index_temp[index]++;
				}
			}
		}
		
		void build_index_map(int seed, bool bisulfite)
		{
			this->bisulfite = bisulfite;
			
			//Populate randomly ordered index
			vector<int> vals;
			DNA::seq2indices(sequence, vals, seed, bisulfite);
			
			//Populate index
			for (int i=0, len=vals.size(); i<len; ++i)
			{
				if (i%100000 == 0)
				{
					cout << (forward ? "+" : "-") << i <<  "            \r" << flush;
				}
				int val = vals[i];
				
				if (val == -1) continue;
				
				index_iterator = index_map.find(val);
				
				if (index_iterator == index_map.end())
				{
					list<int> v;
					index_map[val] = v;
				}
				index_map[val].push_back(i);
			}
			vals.clear();
		}
		
		list<int>* lookup(int index)
		{
			if (index == -1) return NULL;

			index_iterator = index_map.find(index);
			
			if (index_iterator == index_map.end())
			{
				return NULL;
			}
			return &(index_iterator->second);
		}
	};
}
