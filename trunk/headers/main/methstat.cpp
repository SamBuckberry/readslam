#include "../tools/_methstat.h"

void help()
{
	cout << "\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: MethStat ::"
	<<"\n"
	<<"\nA collection of utilities for generating methstats"
	<<"\n  - build      : build a MethStat version of a FastA file using mC data"
	<<"\n  - range      : count all motifs in the specified range around an mC site"
	<<"\n  - count      : count the number of occurences of a specific motif in a ref genome"
	<<"\n  - dump       : dump all the motifs in the specified range around a meth site"
	<<"\n  - profile_up : count meth sites relative to the start of elements"
	<<"\n  - profile_dn : count meth sites relative to the end of elements"
	<<"\n  - profile_in : count meth sites within elements (percentage)"
	<<"\n  - position   : list all the offset positions where a specific motif exists"
	<<"\n  - relative   : list all the motifs relative to positions provided in a file"
	<<"\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: Examples ::"
	<<"\nNote: command keywords are case sensitive"
	<<"\n"
	<<"\n- Show this help information"
	<<"\n    ./methstat"
	<<"\n"
	<<"\n- Building the methstat'd genome"
	<<"\n    ./methstat build ./data.mc ./old.fasta ./new.fasta"
	<<"\n"
	<<"\n- Counting all motifs around an mC site"
	<<"\n    ./methstat range ./genome.fasta offset1 offset2 mc_state"
	<<"\n    ./methstat range ./genome.fasta -2 2 mc"
	<<"\n    ./methstat range ./genome.fasta 2 2 mc"
	<<"\n    ./methstat range ./genome.fasta -5 -1 mc"
	<<"\n    ./methstat range ./genome.fasta 0 3 non_mc"
	<<"\n"
	<<"\n- Counting the number of copies of a motif"
	<<"\n    ./methstat count ./genome.fasta CAGCGACGGT"
	<<"\n"
	<<"\n- Dump raw motifs in the specified range around a meth site"
	<<"\n    ./methstat dump ./genome.fasta -3 3"
	<<"\n"
	<<"\n- Profiling methylation in and around defined elements"
	<<"\n    ./methstat profile_up ./genome.fasta ./elements.slam -1000 10"
	<<"\n    ./methstat profile_dn ./genome.fasta ./elements.slam -10 200"
	<<"\n    ./methstat profile_in ./genome.fasta ./elements.slam"
	<<"\n"
	<<"\n- List all the positions where a motif occurs"
	<<"\n    ./methstat position ./genome.fasta AATGACCG"
	<<"\n"
	<<"\n- List all motifs relative to positions in a provided file"
	<<"\n    ./methstat relative ./genome.fasta ./positions -3 3"
	<<"\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: More Information ::"
	<<"\n"
	<<"\n- Website: http://readslam.revjs.org"
	<<"\n- Contact: Julian Tonti-Filippini (julian@revjs.org)"
	<<"\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<< endl;
	exit(0);
}
void check(int num, bool exact, int argc, vector<string> args)
{
	if ((exact && argc != num) || argc < num)
	{
		cerr << "Error: bad parameter count for: " + args[1] << endl;
		help();
	}
}
int main (int argc, char* const argv[])
{
	vector<string> args;
	
	for (int i=0; i<argc; i++)
	{
		args.push_back(string(argv[i]));
	}
	
	if (argc == 1)
	{
		help();
	}
	
	ReadSlam::MethStat ms;

	if (args[1] == "build")
	{
		check(5,true,argc,args); ms.build(args[2], args[3], args[4]);
	}
	else if (args[1] == "range")
	{
		check(6,true,argc,args); ms.range(args[2], atoi(args[3].c_str()), atoi(args[4].c_str()), args[5] == "mc");
	}
	else if (args[1] == "count")
	{
		check(4,true,argc,args); ms.motif_count(args[2], args[3]);
	}	
	else if (args[1] == "dump")
	{
		check(5,true,argc,args); ms.motif_dump(args[2], atoi(args[3].c_str()), atoi(args[4].c_str()));
	}
	else if (args[1] == "profile_up")
	{
		check(6,true,argc,args); ms.profile_up(args[2], args[3], atoi(args[4].c_str()), atoi(args[5].c_str()));
	}
	else if (args[1] == "profile_dn")
	{
		check(6,true,argc,args); ms.profile_dn(args[2], args[3], atoi(args[4].c_str()), atoi(args[5].c_str()));
	}
	else if (args[1] == "profile_in")
	{
		check(4,true,argc,args); ms.profile_in(args[2], args[3]);
	}
	else if (args[1] == "position")
	{
		check(4,true,argc,args); ms.position(args[2], args[3]);
	}
	else if (args[1] == "relative")
	{
		check(6,true,argc,args); ms.relative(args[2], args[3], atoi(args[4].c_str()), atoi(args[5].c_str()));
	}
	else
	{
		help();
	}
	return 0;
}
