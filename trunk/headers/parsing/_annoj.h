#pragma once

#include "_nonslam.h"

namespace ReadSlam
{	
	//Represents a read in AnnoJ format
	struct ReadAnnoJ : ReadNonSlam
	{
		string assembly;
		string strand;
		int start;
		int end;
		string sequence;
		
		 ReadAnnoJ() { clear(); }
		~ReadAnnoJ() { clear(); }
		
		void clear()
		{
			assembly.clear();
			strand = "+";
			start = 0;
			end = 0;
			sequence.clear();
		}
		
		bool load(ifstream& in)
		{
			return in
				>> assembly
				>> strand
				>> start
				>> end
				>> sequence
			;
		}
		
		void save(ofstream& out)
		{
			out << assembly
				<< '\t' << strand
				<< '\t' << start
				<< '\t' << end
				<< '\t' << sequence
			<< '\n';
		}
				
		void to_slam(Read& slam)
		{
			slam.clear();
			slam.length     = sequence.size();
			slam.mismatches = sequence.size();
			slam.score      = sequence.size() * 255;
			slam.assembly   = assembly;
			slam.forward    = strand == "+";
			slam.position   = start;
			slam.copies     = 1;
			slam.sequence   = (strand == "+") ? sequence : DNA::reverse(sequence);
		}
		
		void from_slam(Read& slam)
		{
			clear();
			assembly  = slam.assembly;
			strand    = slam.forward ? "+" : "-";
			start     = slam.position;
			end       = start + slam.length - 1;
			sequence  = (slam.forward) ? slam.sequence : DNA::reverse(slam.sequence);
		}
	};
}
