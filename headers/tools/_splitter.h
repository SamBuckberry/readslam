#ifndef _READSLAM_SPLITTER
#define _READSLAM_SPLITTER

#include "../common/_common.h"
#include "../parsing/_slam.h"
#include <map>

namespace ReadSlam
{
	//Static code for splitting a .slam file by assembly
	struct Splitter
	{
		static void split(string infile, string outfile)
		{
			ifstream in (infile.c_str());
			
			map<string, ofstream*> files;
			map<string, ofstream*>::iterator files_itr;
			
			files.clear();

			BasicRead read;
			int progress = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << '\r' << flush;
				}
				files_itr = files.find(read.assembly);
				
				if (files_itr == files.end())
				{
					files[read.assembly] = new ofstream((outfile + "." + read.assembly).c_str());
				}
				read.save(*files[read.assembly]);
			}
			in.close();
			
			for (files_itr = files.begin(); files_itr != files.end(); ++files_itr)
			{
				files_itr->second->close();
			}
		}
	};
}
#endif