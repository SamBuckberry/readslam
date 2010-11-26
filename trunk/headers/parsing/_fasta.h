#pragma once

#include "_nonslam.h"

namespace ReadSlam
{	
	//Represents a read in FastA format
	struct ReadFastA : ReadNonSlam
	{
		string name;
		string sequence;
		
		 ReadFastA() { clear(); }
		~ReadFastA() { clear(); }
		
		void clear()
		{
			name.clear();
			sequence.clear();
		}
		
		bool load(ifstream& in)
		{
			clear();

			string line = "";
			
			if (!getline(in,line) || line == "" || line[0] != '>')
			{
				return false;
			}
			name = line.substr(1);
			
			ostringstream s;
			s.clear();
			
			while (getline(in,line))
			{
				if (line[0] == '>')
				{
					//FIXME: backtrace
					break;
				}
				s << line;
			}
			sequence = s.str();
			return true;
		}
		
		void save(ofstream& out)
		{
			out << '>' << name << endl;
			
			for (int i=0, len=sequence.size(); i<len; i+=80)
			{
				out << sequence.substr(i, (i+80 <= len) ? 80 : 80 - (len-i)) << endl;
			}
		}
				
		void to_slam(Read& slam)
		{
			slam.clear();
			slam.name     = name;
			slam.sequence = sequence;
		}
		
		void from_slam(Read& slam)
		{
			clear();
			name     = slam.name;
			sequence = slam.sequence;
		}
	};
}
