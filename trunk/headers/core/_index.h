#ifndef _READSLAM_INDEX
#define _READSLAM_INDEX

#include "_dna.h"

/**
 * Represents an index for a DNA sequence
 */
namespace ReadSlam
{
	class Index
	{
		 Index() { clear(); }
		~Index() { clear(); }
		
		//Load the index from disk
		void load(string infile)
		{
		
		}
	
		//Save the index to disk
		void save(string outfile)
		{
		
		}
		
		//Determine the maximum RAM that will be required given a sequence size (MegaBytes)
		int memory(long size)
		{
			
		}
	
		//Build the index given a DNA sequence
		void build(string& sequence, int seed, bool bisulfite)
		{
		
		}
		
		//Clear the index
		void clear()
		{
			
		}
		
		//Count the number of keys in the index
		int count_keys()
		{
			
		}
			
		//Count how many values there are for a specific index key
		int count_vals(int key)
		{
		
		}
	
		//Get the list of the values in the index for a specified key
		void lookup(int key)
		{
		
		}
	
		//Map a read to the index
		void search(Read& read)
		{
		
		}
	
		//Try aligning a read to the index at the specified position
		void align(Read& read, int pos)
		{
		
		}
	};
}
#endif