#pragma once

#include "_common.h"
#include "_sequence.h"
#include "_read.h"

/**
 * Represents a collection of reads that have been mapped to a single strand in a single assembly
 */
namespace ReadSlam
{
	struct MappedRead
	{
		string sequence;
		string qualities;
		unsigned int position;
	};
	struct MappedReads
	{
		string assembly;
		string strand;
		
		list<MappedRead> reads;
		
		void clear()
		{
			assembly.clear();
			strand.clear();
			reads.clear();
		}
		void load(string infile)
		{
			ifstream in = Files::iget(infile);
			Read read;
			
			while (read.load(in))
			{
				if (read.assembly != assembly) continue;
				if (read.forward != forward) continue;

				MappedRead mapped;
				mapped.sequence = read.sequence;
				mapped.qualities = read.qualities;
				mapped.position = read.position;
				reads.push_back(mapped);
			}
		}
	};
	
	//port to readslam format from fastq
	//collapse on sequence
	//preprocess to remove adapter and trim on quality
	//map reads
	//build stacks
	//
}
