#pragma once

#include "_nonslam.h"

namespace ReadSlam
{
	//Represents a minimalistic ReadSlam read
	struct BasicRead
	{
		int locations;
		int mismatches;
		int score;

		string assembly;
		string strand;
		int position;

		string name;
		int copies;
		string sequence;
		string qualities;
				
		bool load(ifstream& in)
		{
			return in
				>> locations
				>> mismatches
				>> score

				>> assembly
				>> strand
				>> position

				>> name
				>> copies
				>> sequence
				>> qualities
			;			
		}
		void save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';
			
			out << locations
				<< tab << mismatches
				<< tab << score

				<< tab << assembly
				<< tab << strand
				<< tab << position

				<< tab << name
				<< tab << copies
				<< tab << sequence
				<< tab << qualities
			<< end;
		}
	};
}
