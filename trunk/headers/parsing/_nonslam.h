#pragma once

#include "../common/_files.h"
#include "../core/_read.h"

namespace ReadSlam
{
	//Represents a read in non ReadSlam format
	struct ReadNonSlam
	{
		//Clear the read
		void clear()
		{
			
		}
		
		//Load the next read from an open file stream
		bool load(ifstream& in)
		{

		}
		
		//Save the read to an open file stream
		void save(ofstream& out)
		{

		}
		
		//Convert to a ReadSlam read
		void to_slam(Read& read)
		{

		}
		
		//Convert from a ReadSlam read
		void from_slam(Read& read)
		{
			
		}
	};
}
