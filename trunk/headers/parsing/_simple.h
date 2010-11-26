#pragma once

#include "_nonslam.h"

namespace ReadSlam
{
	//Represents a read in simple format
	struct ReadSimple : ReadNonSlam
	{
		string name;
		string sequence;
		string qualities;
		
		 ReadSimple() { clear(); }
		~ReadSimple() { clear(); }
		
		void clear()
		{
			name.clear();
			sequence.clear();
			qualities.clear();
		}
	
		bool load(ifstream& in)
		{
			if (!(in
				>> name
				>> sequence
				>> qualities
			)) return false;
			
			if (sequence.size() != qualities.size())
			{
				cerr << "Simple::load sequence size does not equal qualities size" << endl;
				return false;
			}
			return true;
		}
		
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';
			out << name      << tab;
			out << sequence  << tab;
			out << qualities << end;
		}
		
		void to_slam(Read& slam)
		{
			slam.clear();
			slam.length     = sequence.size();
			slam.mismatches = sequence.size();
			slam.score      = sequence.size() * 255;
			slam.name       = name;
			slam.copies     = 1;
			slam.sequence   = sequence;
			slam.qualities  = qualities;
		}
		
		void from_slam(Read& slam)
		{
			clear();
			name      = slam.name;
			sequence  = slam.sequence;
			qualities = slam.qualities;
		}
	};
}
