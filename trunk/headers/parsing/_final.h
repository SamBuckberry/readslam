#ifndef _READSLAM_READ_FINAL
#define _READSLAM_READ_FINAL

#include "_nonslam.h"

namespace ReadSlam
{
	//Represents a read in 'final' format
	struct ReadFinal : ReadNonSlam
	{
		string name;
		string assembly;
		char strand;
		int start;
		int end;
		string sequence;
		string qualities;
		
		 ReadFinal() { clear(); }
		~ReadFinal() { clear(); }
		
		void clear()
		{
			name.clear();
			assembly.clear();
			strand = '.';
			start = 0;
			end = 0;
			sequence.clear();
			qualities.clear();
		}
	
		bool load(ifstream& in)
		{
			if (!(in
				>> name
				>> assembly
				>> strand
				>> start
				>> end
				>> sequence
				>> qualities
			)) return false;
			
			if (sequence.size() != qualities.size())
			{
				cerr << "Final::load sequence size does not equal qualities size" << endl;
				return false;
			}
			return true;
		}
		
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';
			out 
				<< name      << tab
				<< assembly  << tab
				<< strand    << tab
				<< start     << tab
				<< this->end << tab
				<< sequence  << tab
				<< qualities << end
			;
		}
		
		void to_slam(Read& slam)
		{
			slam.clear();
			slam.name      = name;
			slam.assembly  = assembly;
			slam.forward   = strand == '+';
			slam.position  = start;
			slam.length    = sequence.size();
			slam.sequence  = sequence;
			slam.qualities = qualities;
			
			//These nums can't be computed for sure without a ref genome lookup
			slam.locations = 1;
			slam.copies = 1;
			slam.mismatches = slam.length;
			slam.score = slam.length * 255;
		}
		
		void from_slam(Read& slam)
		{
			clear();
			name      = slam.name;
			assembly  = slam.assembly;
			strand    = slam.forward ? '+' : '-';
			start     = slam.position;
			end       = slam.position + slam.sequence.size() - 1;
			sequence  = slam.sequence;
			qualities = slam.qualities;
		}
	};
}
#endif