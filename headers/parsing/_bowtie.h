#ifndef _READSLAM_READ_BOWTIE
#define _READSLAM_READ_BOWTIE

#include "_nonslam.h"

/*
 * The bowtie aligner outputs each alignment on a separate line. Each line is a collection of 8 fields separated by tabs; 
 * from left to right, the fields are:
 *   1.    Name of read that aligned
 *   2.    Orientation of read in the alignment, - for reverse complement, + otherwise
 *   3.    Name of reference sequence where alignment occurs, or ordinal ID if no name was provided
 *   4.    0-based offset into the reference sequence where leftmost character of the alignment occurs
 *   5.    Read sequence (reverse-complemented if orientation is -)
 *   6.    Read qualities (reversed if orientation is -)
 *   7.    Reserved
 *   8.    Comma-separated list of mismatch descriptors. If there are no mismatches in the alignment, this field is empty.
 *         A single descriptor has the format offset:reference-base>read-base. The offset is expressed as a 0-based offset 
 *         from the high-quality (5') end of the read
 */
namespace ReadSlam
{
	//Represents a read in bowtie format
	struct ReadBowtie : ReadNonSlam
	{
		//Raw fields
		string name;
		string strand;
		string assembly;
		string position;
		string sequence;
		string qualities;
		string reserved;
		string mismatches;

		//Processed values
		int  start;
		int  end;
		int  length;
		int  error_tally;
		int  error_count;
		bool forward;
		
		 ReadBowtie() { clear(); }
		~ReadBowtie() { clear(); }
		
		void clear()
		{
			name.clear();
			strand.clear();
			assembly.clear();
			position.clear();
			sequence.clear();
			qualities.clear();
			reserved.clear();
			mismatches.clear();
			
			start = 0;
			end = 0;
			length = 0;
			error_tally = 0;
			error_count = 0;
			forward = true;
		}
	
		//Load a read from a Bowtie file
		bool load(ifstream& in)
		{
			if (!(in
				>> name
				>> strand
				>> assembly
				>> position
				>> sequence
				>> qualities
				>> reserved
				>> mismatches
			)) return false;
			
			//Processed values
			forward = strand == "+";
			length  = sequence.size();
			start   = atoi(position.c_str());
			end     = start + length - 1;
			
			//Build the error score
			error_tally = 0;
			error_count = 0;

			if (mismatches != "")
			{
				vector<string> errors = Strings::split(mismatches, ',');
			
				for (int i=0, len=errors.size(); i<len; ++i)
				{
					int offset = atoi(Strings::between(errors[i], "", ":").c_str());
					error_tally += (forward ? qualities[offset] : qualities[length-offset-1]);
					error_count++;
				}
			}
			return true;
		}
		
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';
			out << name 
				<< tab << strand
				<< tab << assembly
				<< tab << position
				<< tab << sequence
				<< tab << qualities
				<< tab << reserved
				<< tab << mismatches
			<< end;
		}
		
		//Alter values to suit Anno-J
		bool aj_fix(bool watson)
		{
			if (watson)
			{
				if (strand == "-") return false;
			}
			else
			{
				if (strand == "+") return false;
				sequence = DNA::reverse_complement(sequence);
				qualities = Strings::reverse(qualities);
			}
			assembly = Strings::between(assembly, "chr", "_");
			name = "@" + name;
			return true;
		}
		
		void to_slam(Read& slam)
		{
			slam.clear();
			slam.locations  = 1;
			slam.mismatches = error_count;
			slam.score      = error_tally;

			slam.assembly   = assembly;
			slam.forward    = strand == "+";
			slam.position   = start;

			slam.name       = name;
			slam.copies     = 1;
			slam.sequence   = sequence;
			slam.qualities  = qualities;		

			slam.length     = length;
		}
		
		void from_slam(Read& slam)
		{
			clear();
			name        = slam.name;
			assembly    = slam.assembly;
			strand      = slam.forward;
			start       = slam.position;
			length      = slam.length;
			error_tally = slam.score;
			error_count = slam.mismatches;
			sequence    = slam.sequence;
			qualities   = slam.qualities;
		}
	};
}
#endif