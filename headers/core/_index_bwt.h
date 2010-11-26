#pragma once

#include "_dna.h"
#include "_index.h"
#include <list>
#include "_read.h"

/**
 * Implements the BWT to provide an index for a DNA sequence
 */
namespace ReadSlam
{
	class IndexBWT : Index
	{
		 IndexBWT() { clear(); }
		~IndexBWT() { clear(); }
		
		//Load the index from disk
		void load(string infile)
		{
			//Load a BWT encoded genome
		}
	
		//Save the index to disk
		void save(string outfile)
		{
			//Write out the BWT encoded genome
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
		
		// //Count the number of keys in the index
		// int count_keys()
		// {
		// 	
		// }
		// 	
		// //Count how many values there are for a specific index key
		// int count_vals(int key)
		// {
		// 	
		// }
		// 	
		// //Get the list of the values in the index for a specified key
		// void lookup(int key)
		// {
		// 	
		// }
	
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
