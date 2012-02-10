#pragma once

#include "../common/_common.h"
#include "../parsing/_slam.h"
#include "../core/_dna.h"
#include "_fasta.h"
#include <map>


/**
	Takes a file of mapped reads and counts the number of mCG and mCH sites in each
	against the reference genome at the mapped location. Because the sequence read
	may be only part of the real physical read, a length may be specified based on 
	what is known from a gel spot. If the length specified is shorter than a read 
	then the read length overrides the specified length.
*/
namespace ReadSlam
{
	//Static code for splitting a .slam file by assembly
	struct HMC_Resolver
	{
		struct Assembly
		{
			int size;
			string name;
			string forward;
			string reverse;
		};
		
		map<string, Assembly> genome;
		
		//Load the genome
		void load(string infile)
		{
			cout << "Loading genome" << endl;
			
			FastA::Genome f;
			f.load(infile);
			
			for (int i=0; i<f.assemblies.size(); ++i)
			{
				Assembly a;
				a.size = f.assemblies[i].length;
				a.name = f.assemblies[i].name;
				a.forward = f.assemblies[i].sequence;
				a.reverse = DNA::complement(a.forward);
				
				Strings::replace(a.name, "chr", "");
				Strings::replace(a.name, "_u", "");
				Strings::replace(a.name, " ", "");
				Strings::replace(a.name, ">", "");
				
				genome[a.name] = a;
				
				cout << " - loaded " << a.name << endl;
			}
		}
		
		//Take a file of reads and generate stats
		//Use a fraglen of 0 to force fraglen to be equal to readlen.
		void process(string infile, int fraglen)
		{
			//Sanity check
			if (fraglen > 500)
			{
				cerr << "Fragment length must be <= 500" << endl;
				return;
			}
			
			//Initialize counters
			unsigned int progress = 0;

			unsigned int stats_cg = 0;
			unsigned int stats_ch = 0;
			unsigned int stats_mcg = 0;
			unsigned int stats_mch = 0;
			
			unsigned int stats_reads = 0;
			unsigned int stats_reads_error = 0;
			unsigned int stats_reads_ambig = 0;
			unsigned int stats_reads_cg = 0;
			unsigned int stats_reads_ch = 0;
			unsigned int stats_reads_cg_single = 0;
			unsigned int stats_reads_ch_single = 0;
			
			//Create outfiles
			string outfile_cg = infile + "_unambiguous_cg";
			string outfile_ch = infile + "_unambiguous_ch";
			
			ofstream out_cg (outfile_cg.c_str());
			ofstream out_ch (outfile_ch.c_str());
			
			//Process reads from the file
			ifstream in (infile.c_str());
			BasicRead read;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << "  " << progress << '\r' << flush;
				}
				string ref;
				int len = read.sequence.size();
				if (len <= 0) continue;
				
				int reflen = fraglen > len ? fraglen : len;
				
				//Skip if there might be an out of range error
				int a = (read.strand == "+") ? read.position : read.position + len - reflen - 1;
				int b = a + reflen + 1;

				if (a < 0 || b >= genome[read.assembly].size) continue;
				
				if (read.strand == "+")
				{
					ref = genome[read.assembly].forward.substr(a,reflen+1);
				}
				else
				{
					ref = DNA::reverse(genome[read.assembly].reverse.substr(a,reflen+1));
				}
				
				//Generate stats
				int mcg = 0; 
				int mch = 0;
				
				for (int i=0; i<reflen; ++i)
				{
					if (ref[i] == 'C')
					{
						(ref[i+1] == 'G') ? stats_cg++ : stats_ch++;
						
						if (i < len)
						{
							if (read.sequence[i] != 'C') continue;
						}
						(ref[i+1] == 'G') ? mcg++ : mch++;
					}
				}
				stats_mcg += mcg;
				stats_mch += mch;
				
				stats_reads++;

				if (mcg == 0 && mch == 0) stats_reads_error++;
				if (mcg  > 0 && mch  > 0) stats_reads_ambig++;
				if (mcg  > 0 && mch == 0) stats_reads_cg++;
				if (mcg == 0 && mch  > 0) stats_reads_ch++;
				if (mcg == 1 && mch == 0) stats_reads_cg_single++;
				if (mcg == 0 && mch == 1) stats_reads_ch_single++;
				
				//Save reads if needed
				if (mcg  > 0 && mch == 0) read.save(out_cg);
				if (mcg == 0 && mch  > 0) read.save(out_ch);
			}
			in.close();
			out_cg.close();
			out_ch.close();
			
			//Print results
			cout << "Total reads: " << stats_reads << endl;
			cout << "Error reads: " << stats_reads_error << endl;
			cout << "Ambiguous reads: " << stats_reads_ambig << endl;
			cout << "hmCG reads: " << stats_reads_cg << endl;
			cout << "hmCH reads: " << stats_reads_ch << endl;
			cout << "hmCG reads single: " << stats_reads_cg_single << endl;
			cout << "hmCH reads single: " << stats_reads_ch_single << endl;
			cout << "mCG / CG sites: " << stats_mcg << " / " << stats_cg << endl;
			cout << "mCH / CH sites: " << stats_mch << " / " << stats_ch << endl;
		}
	};
}
