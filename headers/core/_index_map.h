#ifndef _READSLAM_INDEX
#define _READSLAM_INDEX

#include "_dna.h"
#include "_index.h"

/**
 * Represents an index for a DNA sequence
 */
namespace ReadSlam
{
	struct IndexMap : Index
	{
		//The index is a map of lists
		map<int, list<int>* > index;
		map<int, list<int>* >::iterator iterator;		
		
		bool bisulfite;
		int seed;

		 IndexMap() { clear(); }
		~IndexMap() { clear(); }
		
		//Load the index from disk
		void load(string infile)
		{
		
		}
	
		//Save the index to disk
		void save(string outfile)
		{
		
		}
		
		//Clear the index
		void clear()
		{
			index.clear();
			bisulfite = false;
			seed = 0;
		}
		
		//Count the number of keys in the index
		int size()
		{
			return index.size();
		}

		//Count how many entries there are for a specific index key
		int count(int key)
		{
			if (key < 0) return 0;
			iterator.find(key);
			if (iterator == index.end()) return 0;
			return iterator->second->size();
		}		
	
		//Determine the maximum RAM that would be required given a sequence size and a seed value
		int memory(long sequence_size, int target_seed)
		{
			int size_idx = (int)(sequence_size * sizeof(int) / 1000000);
			int size_seq = (int)(sequence_size * sizeof(char) / 1000000);
			return size_idx + size_seq;
		}

		//Build the index given a DNA sequence
		void build(string& sequence, int seed, bool bisulfite)
		{
			if (seed < 1 || seed > 15)
			{
				cerr << "Seed must be between 1 and 15" << endl;
				return;
			}
			this->seed = seed;
			this->bisulfite = bisulfite;
			
			//Generate all index keys from sequence
			vector<int> indices;
			DNA::seq2indices(sequence, indices, seed, bisulfite);
			
			//Populate index
			for (int i=0, limit=indices.size(); i<limit; ++i)
			{
				if (i%1000000 == 0)
				{
					cout << i <<  "\r" << flush;
				}
				add(indices[i], i);
			}
			indices.clear();
		}
		
		//Add an entry to the index
		void add(int key, int value)
		{
			if (key < 0 || value < 0) return;
			
			if (count(key) == 0)
			{
				index[key] = new list<int>;
			}
			index[key]->push_back(value);
		}
	
		//Get the list of the values in the index for a specified key
		list<int>* lookup(int key)
		{
			if (key < 0) return NULL;
			iterator.find(key);
			if (iterator == index.end()) return NULL;
			return iterator->second;
		}			
	
		//Map a read to the index
		void map_read(Read& read)
		{
			list<int>* locations;
			list<int>::iterator it;
			
			for (int i=0; i<read.length; ++i)
			{
				locations = lookup(read.indices[i].val);
				
				if (locations == NULL)
				{
					if (read.mismatches == 0) return;
					continue;
				}
				
				int pos_read = read.indices[i].pos;
		
				for (it = locations->begin(); it != locations->end(); ++it)
				{
					long pos_genome = *it - pos_read;
					
					if (pos_genome < 0 || pos_genome + length > s.length)
					{
						continue;
					}
					align(read, pos_genome);
				}
				break;
			}
		}
	
		//Try aligning a read to the index at the specified position
		void align(Read& read, int pos)
		{
		
		}
	};
}
#endif