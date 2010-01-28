#ifndef _READSLAM_INDEX
#define _READSLAM_INDEX

#include "_dna.h"
#include "_index.h"
#include <list>
#include "_read.h"

/**
 * Represents an index for a DNA sequence
 * IndexVector uses vector for storing index information
 * It can be sparse, but it is fast
 */
namespace ReadSlam
{
	struct IndexVector
	{
		//The index is a collection of vectors
		vector<int> index;
		vector<int> counts;
		vector<int> offsets;
		
		bool bisulfite;
		int seed;
		int max;
		int length;

		 IndexVector() { clear(); }
		~IndexVector() { clear(); }
		/*
		//Load the index from disk
		void load(string infile)
		{
			clear();
			
			ifstream in (infile.c_str(), ios::in | ios::binary | ios::ate);
			
			if (in.good())
			{
				int size = in.tellg() / 3;
				
				index.resize(size);
				counts.resize(size);
				offsets.resize(size);
				
				in.seekg(0, ios::beg);
				in.read((char*)index, size);
				in.read((char*)counts, size);
				in.read((char*)offsets, size);
			}
		    in.close();
		}
	
		//Save the index to disk
		void save(string outfile)
		{
			ofstream out (outfile.c_str(), ios::out | ios::binary);
			
			int size = index.size();
			
			if (out.good())
			{
				out.write((const char*)index, size);
				out.write((const char*)counts, size);
				out.write((const char*)offsets, size);
			}
		    out.close();
		}
		*/
		//Clear the index
		void clear()
		{
			index.clear();
			counts.clear();
			offsets.clear();

			max = 0;
			seed = 0;
			bisulfite = false;
		}
		
		//Count the number of keys in the index
		int count_keys()
		{
			return index.size();
		}
		
		//Count how many entries there are for a specific index key
		int count_vals(int key)
		{
			return (key < 0 || key > max-1) ? 0 : counts[key];
		}
					
		//Determine the maximum RAM that would be required given a sequence size and a seed value
		int memory(long sequence_size, int target_seed)
		{
			if (target_seed > 15 || target_seed < 1 || sequence_size < 0) return -1;
			
			int base = 4;
			long max = (long)(pow((double)base, (double)target_seed));
			long size_index = (sequence_size * sizeof(int)) / 1000000;
			long size_counts = (max * sizeof(int)) / 1000000;
			long size_offsets = (max * sizeof(int)) / 1000000;

			return size_index + size_counts + size_offsets;
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
			this->max = (int)pow((double)4, (double)seed);
			this->length = sequence.size();

			//Initialize vectors
			vector<int> temp;
			vector<int> random;

			random.resize(length,-1);
			index.resize(length,-1);
			counts.resize(this->max,0);
			offsets.resize(this->max,0);
			temp.resize(this->max,0);

			//Populate randomly ordered index
			DNA::seq2indices(sequence, random, seed, bisulfite);

			//Populate counts
			for (int i=0; i<length; ++i)
			{
				int index = random[i];

				if (index != -1)
				{
					counts[index]++;
				}
			}

			//Populate offsets
			for (int i=0, offset=0; i<max; ++i)
			{
				offsets[i] = offset;
				offset += counts[i];
			}

			//Populate ordered index
			for (int i=0; i<length; ++i)
			{
				if (i%1000000 == 0)
				{
					cout << i <<  "\r" << flush;
				}
				int value = random[i];

				if (value != -1)
				{
					int offset = offsets[value] + temp[value];
					index[offset] = i;
					temp[value]++;
				}
			}
			temp.clear();
			random.clear();
		}
	
		//Get the list of the values in the index for a specified key
		list<int> lookup(int key)
		{
			list<int> positions;

			if (count_vals(key) == 0) positions;
			
			int offset = offsets[key];
			int count = counts[key];
			
			for (int i=0; i<count; ++i)
			{
				positions.push_back(index[offset+i]);
			}
			return positions;
		}
	
		//Map a read to the index
		void search(Read& read)
		{
			for (int i=0; i<read.length; ++i)
			{
				int idx = read.indices[i].val;
				if (idx == -1) continue;

				int count = counts[idx];
				if (count == 0)
				{
					if (read.mismatches == 0) return;
					continue;
				}
				int offset = offsets[idx];
				int pos_read = read.indices[i].pos;

				for (int p=0; p<count; ++p)
				{
					int pos_genome = index[offset + p] - pos_read;
					
					if (pos_genome < 0 || pos_genome + read.length > length)
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