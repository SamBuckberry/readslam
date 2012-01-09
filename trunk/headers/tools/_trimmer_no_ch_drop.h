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
			
			int dropped = 0;
			int trimmed = 0;
			
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
				
				//Apply filtering rules
				int error = 0;
				int i = 0;
				
				for (i=0; i<len; ++i)
				{
					if      (ref[i] == 'C' && read.sequence[i] == 'T') continue;
					else if (ref[i] == 'C' && read.sequence[i] != 'C') error++;
					else if (ref[i] == read.sequence[i]) continue;
					else    error++;
					
					if (error > 3) break;
				}
				
				//Trim a read with errors
				if (error > 3)
				{
					read.sequence = read.sequence.substr(0,i);

					if (read.strand == "-")
					{
						read.position = read.position + (len - read.sequence.size());
					}
					trimmed++;
				}
				
				//Save the read if it is good enough
				if (read.sequence.size() > 20)
				{
					read.save(out);
				}
				else
				{
					dropped++;
				}				
			}
			out.close();
			in.close();
			cout << "Total " << progress << endl;
			cout << "Trimmed " << trimmed << endl;
			cout << "Dropped " << dropped << endl;
		}
	};
}
