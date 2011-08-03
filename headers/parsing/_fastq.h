#pragma once

#include "_nonslam.h"

namespace ReadSlam
{
	//Represents a read in FastQ format
	struct ReadFastQ : ReadNonSlam
	{
		string sequence_header;
		string sequence;
		string qualities_header;
		string qualities;

		 ReadFastQ() { clear(); }
		~ReadFastQ() { clear(); }
		
		void clear()
		{
			sequence_header.clear();
			sequence.clear();
			qualities_header.clear();
			qualities.clear();
		}
		
		bool load(ifstream& in)
		{
			clear();
			
			string line = "";

			for (int i=0; i<4; ++i)
			{
				if (!getline(in,line))
				{
					cerr << "End of file, or bad input" << endl;
					return false;
				}
				if (line == "")
				{
					cerr << "Bad line (blank)" << endl;
					return false;
				}
				
				switch(i)
				{
					case 0:
						if (line[0] != '@')
						{ 
							cerr << "Bad sequence header line: " << line << endl;							
							return false; 
						}
						
						//Replace spaces in the ID with colons
						for (int j=0; j<line.length(); j++)
						{
							if (line[j] == ' ') line[j] = ':';
						}
						sequence_header = line;
					break;
					
					case 1:
						sequence = line;
					break;
		
					case 2:
						if (line[0] != '+')
						{ 
							cerr << "Bad qualities header line: " << line << endl;
							return false; 
						}
						qualities_header = line;
					break;
					
					case 3:
						qualities = line;
						if (sequence.size() != qualities.size())
						{
							cerr << "Sequence size does not match qualities size: " << line << endl;
							return false; 
						}
					break;
				}
			}
			return true;
		}
		
		void save(ofstream& out)
		{
			char end = '\n';
			out << sequence_header << end;
			out << sequence << end;
			out << qualities_header << end;
			out << qualities << end;
		}
		
		void to_slam(Read& slam)
		{
			slam.clear();
			slam.length     = sequence.size();
			slam.mismatches = sequence.size();
			slam.score      = sequence.size() * 255;
			slam.name       = Strings::ltrim(sequence_header, '@');
			slam.copies     = 1;
			slam.sequence   = sequence;
			slam.qualities  = qualities;
		}
		
		void from_slam(Read& slam)
		{
			sequence_header  = "@" + slam.name;
			sequence         = slam.sequence;
			qualities_header = "+" + slam.name;
			qualities        = slam.qualities;
		}
	};
}
