#pragma once

#include "../common/_common.h"
#include "../parsing/_slam.h"
#include "../core/_dna.h"
#include "_fasta.h"
#include <map>


/**
 NOTE: also see _preprocess.h for a trimming process
*/

/*

- Comparing reads to hg18 reference sequence. Has two filtering components:

- Quality filter:
	- Scan through read to identify errors (tolerating read T's in ref C positions).
	- If >3 errors found, trim the read to the base before the 4th error.
	- If after trimming the length is < 40 nt, drop the read.
	- If read has > 20% errors, drop the read.

- Non-converted read filter:
	- Scan through read to identify C's in non-CG context.
	- Require that the C's are at C reference positions (i.e. we tolerate the miscalling of A's as C's).
	- If >3 C's are present in non-CG context, drop the read.

*/
namespace ReadSlam
{
	//Static code for splitting a .slam file by assembly
	struct Trimmer
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
		
		void trim(string infile, string outfile)
		{
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());
			
			BasicRead read;
			int progress = 0;
			
			//Counts
			int count_cg = 0;
			int count_ch = 0;
			
			int count_mcg = 0;
			int count_mch = 0;
			
			int count_reads = 0;
			int count_reads_ambig = 0;
			int count_reads_cg = 0;
			int count_reads_ch = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << '\r' << flush;
				}
				string ref;
				int len = read.sequence.size();
				if (len <= 0) continue;
				
				if (read.strand == "+")
				{
					if (read.position + len + 1 >= genome[read.assembly].size-2) continue;
					ref = genome[read.assembly].forward.substr(read.position,len+1);
				}
				else
				{
					if (read.position == 0) continue;
					ref = DNA::reverse(genome[read.assembly].reverse.substr(read.position-1,len+1));
				}
				
				//Generate counts
				int cg = 0;
				int ch = 0;
				
				for (int i=0; i<len; ++i)
				{
					if (ref[i] == 'C')
					{
						(ref[i+1] == 'G') ? count_cg++ : count_ch++;
						 
						if (read.sequence[i] != 'C') continue;
						
						(ref[i+1] == 'G') ? cg++ : ch++;
					}
				}
				count_mcg += cg;
				count_mch += ch;
				
				count_reads++;

				if (cg > 0 && ch > 0) count_reads_ambig++;
				else (cg > 0) ? count_reads_cg++ : count_reads_ch++;
				
				if (ch > 0 && cg == 0)
				{
					read.save(out);
				}
			}
			out.close();
			in.close();
			
			//Print results
			cout << "Total reads: " << count_reads << endl;
			cout << "Ambiguous reads: " << count_reads_ambig << endl;
			cout << "hmCG reads: " << count_reads_cg << endl;
			cout << "hmCH reads: " << count_reads_ch << endl;
			cout << "mCG / CG sites: " << count_mcg << " / " << count_cg << endl;
			cout << "mCH / CH sites: " << count_mch << " / " << count_ch << endl;
		}
		
		//Take a file of reads and generate stats
		void process(string infile, int fraglen)
		{
			//Sanity check
			if (fraglen > 200)
			{
				cerr << "Fragment length must be <= 200" << endl;
				return;
			}
			
			ifstream in (infile.c_str());
			
			BasicRead read;
			int progress = 0;
			
			//Counts
			struct s {
				unsigned int cg;
				unsigned int ch;
				
				unsigned int mcg;
				unsigned int mch;
				
				unsigned int reads;
				unsigned int reads_error;
				unsigned int reads_ambig;
				unsigned int reads_cg;
				unsigned int reads_ch;
				unsigned int reads_cg_single;
				unsigned int reads_ch_single;
			} stats;
			
			stats.cg = 0;
			stats.ch = 0;
			
			stats.mcg = 0;
			stats.mch = 0;
			
			stats.reads = 0;
			stats.reads_error = 0;
			stats.reads_ambig = 0;
			stats.reads_cg = 0;
			stats.reads_ch = 0;
			stats.reads_cg_single = 0;
			stats.reads_ch_single = 0;
			
			while (read.load(in))
			{
				if (++progress % 100000 == 0)
				{
					cout << " " << progress << '\r' << flush;
				}
				string ref;
				int len = read.sequence.size();
				if (len <= 0) continue;
				
				if (read.strand == "+")
				{
					if (read.position + fraglen + 1 >= genome[read.assembly].size-2) continue;
					ref = genome[read.assembly].forward.substr(read.position,fraglen+1);
				}
				else
				{
					if (read.position < fraglen) continue;
					ref = DNA::reverse(genome[read.assembly].reverse.substr(read.position-1,fraglen+1));
				}
				
				//Generate stats
				int cg = 0;
				int ch = 0;
				
				for (int i=0; i<fraglen; ++i)
				{
					if (ref[i] == 'C')
					{
						(ref[i+1] == 'G') ? stats.cg++ : stats.ch++;
						
						if (i < len)
						{
							if (read.sequence[i] != 'C') continue;
						}
						(ref[i+1] == 'G') ? cg++ : ch++;
					}
				}
				stats.mcg += cg;
				stats.mch += ch;
				
				stats.reads++;

				if (cg > 0 && ch > 0) stats.reads_ambig++;
				if (cg == 0 && ch == 0) stats.reads_error++;
				if (cg > 0 && ch == 0) stats.reads_cg++;
				if (ch > 0 && cg == 0) stats.reads_ch++;
				if (cg == 1 && ch == 0) stats.reads_cg_single++;
				if (ch == 1 && cg == 0) stats.reads_ch_single++;
			}
			in.close();
			
			//Print results
			cout << "Total reads: " << stats.reads << endl;
			cout << "Error reads: " << stats.reads_error << endl;
			cout << "Ambiguous reads: " << stats.reads_ambig << endl;
			cout << "hmCG reads: " << stats.reads_cg << endl;
			cout << "hmCH reads: " << stats.reads_ch << endl;
			cout << "hmCG reads single: " << stats.reads_cg_single << endl;
			cout << "hmCH reads single: " << stats.reads_ch_single << endl;
			cout << "mCG / CG sites: " << stats.mcg << " / " << stats.cg << endl;
			cout << "mCH / CH sites: " << stats.mch << " / " << stats.ch << endl;
		}
	};
}
