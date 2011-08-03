#pragma once

#include "../common/_common.h"
#include "../parsing/_fastq.h"

/**
 * Code for pre-processing reads
 */
namespace ReadSlam
{
	struct PreProcessor
	{
		long before;
		long after;
		int count;
		int trimmed_adapter;
		int trimmed_quality;
		string adapter;
		
		//Trim reads in a FastQ input file, writing to a FastQ output file
		void trim(string adapter, string infile, string outfile)
		{
			before = 0;
			after = 0;
			count = 0;
			trimmed_adapter = 0;
			trimmed_quality = 0;
			this->adapter = adapter;
			
			ifstream in;
			in.open(infile.c_str());
			
			ofstream out;
			out.open(outfile.c_str());
			
			ReadFastQ f;
			
			while (f.load(in))
			{
				count++;
				before += f.sequence.size();
				trim_on_quality(f);
				trim_on_adapter(f);
				convert_cytosines(f);
				
				if (f.sequence.size() <= 20)
				{
					f.sequence  = "NNNNNNNNNNNNNNNNNNNN";
					//f.qualities = "BBBBBBBBBBBBBBBBBBBB"; //old 64 scoring
					f.qualities = "####################"; //phred 33 scoring

				}
				f.save(out);
				after += f.sequence.size();
			}
			out.close();
			in.close();
			
			//Print results: total, % quality trimmed, % adapter trimmed, average length
			cout << count << '\t';
			cout << (trimmed_quality / (count/100)) << '\t';
			cout << (trimmed_adapter / (count/100)) << '\t';
			cout << (after / count) << endl;
		}
		
		//Trim runs of low quality calls from the end of a read.
		//phred 33: # maps to a quality of 2
		//illum 64: P maps to a quality of 16
		void trim_on_quality(ReadFastQ& f)
		{
			//Count the number of bases from the 3' end that are below threshold
			int c = 0;
			
			for (int i = f.qualities.size() - 1; i >= 0; --i)
			{
				if (f.qualities[i] > 35) break;
				c++;
			}
			
			//Trim the read if necessary
			if (c > 0)
			{
				f.sequence = f.sequence.substr(0, f.sequence.length() - c);
				f.qualities = f.qualities.substr(0, f.qualities.length() - c);
				trimmed_quality++;
			}
		}		
		
		//Trim adapter sequence from the end of the read
		void trim_on_adapter(ReadFastQ& f) 
		{
			//string adapter = "AGATCGGAAGAGCTCGTATGCCGTCTTCTGCTTG";
			
			int size_adapter = adapter.size();
			int size_read = f.sequence.size();

			for (int i=0; i<size_read - 3; ++i)
			{
				bool match = true;
				
				for (int j=0; j<size_adapter; ++j)
				{
					if (i+j >= size_read) break;
					if (f.sequence[i+j] != adapter[j])
					{ 
						match = false; 
						break;
					}
				}
				if (match)
				{
					f.sequence = f.sequence.substr(0, i);
					f.qualities = f.qualities.substr(0, i);
					trimmed_adapter++;
					break;
				}
			}
			
			// for (int i=adapter.size(), len=f.sequence.size(); i > 2; --i)
			// {
			// 	if (i > len) i = len;
			// 	
			// 	if (f.sequence.substr(len-i, i) == adapter.substr(0,i))
			// 	{
			// 		f.sequence = f.sequence.substr(0, len-i);
			// 		f.qualities = f.qualities.substr(0, len-i);
			// 		trimmed_adapter++;
			// 		return;
			// 	}
			// }
		}
		
		//Convert cytosines in the read into thymines
		void convert_cytosines(ReadFastQ& f)
		{
			for (int i=f.sequence.size()-1; i>=0; --i)
			{
				if (f.sequence[i] == 'C')
				{
					f.sequence[i] = 'T';
				}
			}
		}
	};
}
