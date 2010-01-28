#ifndef _READSLAM_COLLAPSER_CLONE
#define _READSLAM_COLLAPSER_CLONE

#include "../common/_common.h"
#include "../core/_read.h"
#include <map>

namespace ReadSlam
{
	//Collapse clonal reads in a .slam file. (One option assumes pre-sorting, the other does not)
	struct CollapseSite
	{
		int score;
		int count;
		string id;
		
		CollapseSite()
		{
			score = 0;
			count = 0;
			id = "";
		}
	};
	struct Collapser
	{	
		static void collapse_clones(string infile, string outfile)
		{
			//index, position
			map<string, map<int, CollapseSite> > index;
			map<string, map<int, CollapseSite> >::iterator it_assembly;
			map<int, CollapseSite>::iterator it_location;

			ifstream in;
			ofstream out;
			
			Read read;
			string line;
		
			in.open(infile.c_str());
			
			int progress = 0;
			int dropped = 0;
			int deepest = 0;
			int sum = 0;
			int total = 0;
			
			//Build a store of all positions
			while (getline(in, line))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << "\r" << flush;
				}
				if (!read.from_string(line)) continue;
				if (read.copies == 0) read.copies = 1;

				int score = 0;
				
				for (int i=0; i<read.length; ++i)
				{
					score += (int)read.qualities[i];
				}
				
				it_assembly = index.find(read.assembly);
				
				if (it_assembly == index.end())
				{
					map<int, CollapseSite> sites;
					index[read.assembly] = sites;
				}
				
				int pos = read.forward ? read.position : -(read.position + read.length);
				
				it_location = index[read.assembly].find(pos);
				
				if (it_location == index[read.assembly].end())
				{
					CollapseSite c;
					c.score = score;
					c.count = read.copies;
					c.id = read.name;
					index[read.assembly][pos] = c;
				}
				else
				{
					if (score > it_location->second.score)
					{
						it_location->second.score = score;
						it_location->second.id = read.name;
					}
					it_location->second.count += read.copies;
				}
			}
			in.close();
			
			cout << "Finished populating index. Now dropping clones" << endl;
			
			//Read through the input again and drop the clones
			in.open(infile.c_str());
			out.open(outfile.c_str());
			progress = 0;
			
			while (getline(in, line))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << "\r" << flush;
				}				
				if (!read.from_string(line)) continue;
				
				int pos = read.forward ? read.position : -(read.position + read.length);
				
				CollapseSite site = index[read.assembly][pos];

				if (site.id == read.name)
				{
					read.copies = site.count;
					read.save(out);
					
					if (read.copies > 1)
					{
						++total;
						sum += read.copies;
					}
					if (read.copies > deepest)
					{
						deepest = read.copies;
					}
					//index[read.assembly].erase(it_location);
				}
				else
				{
					++dropped;
				}
			}
			out.close();
			in.close();
			
			cout << "Collapse complete" << endl;
			cout << " - clonal reads dropped: " << dropped << endl;
			cout << " - deepest clone stack: " << deepest << endl;
			cout << " - average clone stack: " << sum / total << endl;
		}
		
		//Assumes that the incoming file contains reads sorted by clone order
		static void collapse_sorted_clones(string infile, string outfile)
		{
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());
			
			Read prev;
			Read now;
			string line;
		
			int progress = 0;
			int dropped = 0;
			int deepest = 0;
			int sum = 0;
			int total = 0;
			int score = 0;
			
			//Build a store of all positions
			while (now.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << "\r" << flush;
				}
				//if (!now.from_string(line)) continue;
				if (now.copies == 0) now.copies = 1;

				int score_temp = 0;
				
				for (int i=0; i<now.length; ++i)
				{
					score_temp += (int)now.qualities[i];
				}

				if (progress == 1)
				{
					prev = now;
					score = score_temp;
					continue;
				}
				
				if (prev.clonal(now))
				{
					if (score_temp <= score)
					{
						prev.copies += now.copies;
					}
					else
					{
						now.copies += prev.copies;
						prev = now;
					}
					if (prev.copies > deepest)
					{
						deepest = prev.copies;
					}
					dropped++;
				}
				else
				{
					if (prev.copies > 1)
					{
						sum += prev.copies;
						total++;
					}
					prev.save(out);
					prev = now;
					score = score_temp;
				}
			}
			
			if (prev.copies > 1)
			{
				sum += prev.copies;
				total++;
			}			
			prev.save(out);
			out.close();
			in.close();
			
			cout << "Collapse complete" << endl;
			cout << " - clonal reads dropped: " << dropped << endl;
			cout << " - deepest clone stack: " << deepest << endl;
			cout << " - average clone stack: " << sum / total << endl;
		}		
	};
}
#endif