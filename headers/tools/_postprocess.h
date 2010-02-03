#ifndef _READSLAM_POST
#define _READSLAM_POST

#include "../common/_common.h"
#include "../parsing/_fastq.h"
#include "../parsing/_bowtie.h"
#include "../core/_read.h"
#include <map>

/**
 * Code for post-processing bowtie reads
 */
namespace ReadSlam
{	
	struct PostProcessor
	{
		//Header, sequence
	    map<string, string> original;
		map<string, Read> reads;
		
		ReadSlam::ReadFastQ fq;
		ReadSlam::ReadBowtie bt;
		
		//counters
		int total;
		int not_found;
		int unique;
		int multi;
		int fresh;
		int illogical;
		int bumped;
		int skipped;
		int merged;
		
		void process(string infile_original, string infile_f, string infile_r, string outfile)
		{
			total = 0;
			not_found = 0;
			unique = 0;
			multi = 0;
			fresh = 0;
			illogical = 0;
			bumped = 0;
			skipped = 0;
			merged = 0;
			
			load_fastq(infile_original);
			load_bowtie(infile_f, true);
			load_bowtie(infile_r, false);
			save(outfile);
			
			//For debugging
			// cout << "Total: " << total << endl;
			// cout << "Not found: " << not_found << endl;	
			// cout << "Unique: " << unique << endl;
			// cout << "Multi: " << multi << endl;
			// cout << "Fresh: " << fresh << endl;
			// cout << "Illogical: " << illogical << endl;
			// cout << "Bumped: " << bumped << endl;
			// cout << "Skipped: " << skipped << endl;
			// cout << "Merged: " << merged << endl;	
			cout << unique << '\t' << multi << endl;
		}
		
		void load_fastq(string infile)
		{
			ifstream in (infile.c_str(), fstream::in);

			while (fq.load(in))
			{
				original[fq.sequence_header] = fq.sequence;
			}
			//cout << "The number of FastQ lines read in is: " << original.size() << endl;
			in.close();
		}
		
		void load_bowtie(string infile, bool forward)
		{
			ifstream in (infile.c_str(), fstream::in);

			while (bt.load(in))
			{
				total++;
				
				if (!bt.aj_fix(forward))
				{
					illogical++;
					continue;
				}
				Read read;
				bt.to_slam(read);
				
				map<string,Read>::iterator it = reads.find(bt.name);
				
				if (it != reads.end())
				{
					//skip if worse
					if (reads[bt.name].score < read.score)
					{
						skipped++;
					}
					//replace if better
					else if (reads[bt.name].score > read.score)
					{
						bumped++;
						reads[bt.name] = read;
					}
					//multi if the same
					else
					{
						merged++;
						reads[bt.name].locations++;
					}
				}
				else
				{
					fresh++;
					reads[bt.name] = read;
				}
			}
			in.close();
		}
		
		//Convert cytosines in the read into thymines
		void convert_cytosines(string key)
		{
			reads[key].sequence = original[key].substr(0,reads[key].length);
		}
		
		//Save a read to the FastQ output file
		void save(string outfile)
		{
			ofstream out_aj ((outfile + "_aj").c_str(), fstream::out);
			ofstream out_final ((outfile + "_final").c_str(), fstream::out);
			
			char tab = '\t';

			for (map<string, Read>::iterator it = reads.begin(); it != reads.end(); ++it)
			{
				//Replace the sequence
				map<string,string>::iterator it2 = original.find(it->first);
				
				if (it2 == original.end())
				{
					not_found++;
					continue;
				}
				if (it->second.locations > 1)
				{
					multi++;
					continue;
				}
				unique++;
				it->second.sequence = it2->second.substr(0, it->second.length);
				
				//Save AJ
				char TAB = '\t';
				out_aj << it->second.assembly << TAB;
				out_aj << (it->second.forward ? "+" : "-") << TAB;
				out_aj << it->second.position << TAB;
				out_aj << it->second.position + it->second.length - 1 << TAB;
				out_aj << it->second.sequence << endl;
		
				//Save final
				out_final << it->second.name << TAB;
				out_final << it->second.assembly << TAB;
				out_final << (it->second.forward ? "+" : "-") << TAB;
				out_final << it->second.position << TAB;
				out_final << it->second.position + it->second.length - 1 << TAB;
				out_final << it->second.sequence << TAB;
				out_final << it->second.qualities << endl;
			}
			out_final.close();
			out_aj.close();
		}
	};
}
#endif