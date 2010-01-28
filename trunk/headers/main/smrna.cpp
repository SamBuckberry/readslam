#include "../tools/_smrna.h"

void help()
{
	cout << "\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: Analysis ::"
	<<"\n"
	<<"\nVarious analysis tools"
	<<"\n  - motif_genome  : motif count within a genome"
	<<"\n  - motif_regions : motif count within a genome within specified coordinates"
	<<"\n  - randomize     : produce a file of reads randomly pulled from the genome"
	<<"\n  - meth_genome   : anaylze meth motifs for a genome"
	<<"\n  - meth_reads    : analyze meth motifs for reads against a genome"
	<<"\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: Examples ::"
	<<"\nNote: command keywords are case sensitive"
	<<"\n"
	<<"\n - Search for a motif in the genome"
	<<"\n    ./analysis motif_genome ./genome.fasta ATTTTAG"
	<<"\n"
	<<"\n - Regions file is format assembly\\tstart\\tend\\n"
	<<"\n    ./analysis motif_regions ./genome.fasta ./regions ATTTTAG"
	<<"\n"
	<<"\n - Takes the reads from a file and produces randomly selected equivalents\n"
	<<"\n    ./analysis randomize genome.fa reads.in reads.random"
	<<"\n"
	<<"\n    ./analysis meth_genome genome.fa"
	<<"\n    ./analysis meth_reads genome.fa reads.in"
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

	ReadSlam::Analysis a;

	if (args[1] == "motif_genome")
	{
		check(4,true,argc,args);
		cout << a.motif_count_in_genome(args[2], args[3]) << endl;
	}
	else if (args[1] == "motif_regions")
	{
		check(5,true,argc,args);
		cout << a.motif_count_in_regions(args[2], args[3], args[4]) << endl;
	}
	else if (args[1] == "randomize")
	{
		check(5,true,argc,args);
		a.randomize(args[2], args[3], args[4]);
	}
	else if (args[1] == "meth_genome")
	{
		check(3,true,argc,args);
		a.meth_genome(args[2]);
	}
	else if (args[1] == "meth_reads")
	{
		check(4,true,argc,args);
		a.meth_reads(args[2], args[3]);
	}
	else if (args[1] == "hm_stat")
	{
		check(5,true,argc,args);
		a.hm_stat(args[2], args[3], args[4]);
	}
	else if (args[1] == "hm_randomize")
	{
		check(5,true,argc,args);
		a.hm_randomize(args[2], args[3], args[4]);
	}
	else if (args[1] == "hm_plot")
	{
		check(3,true,argc,args);
		a.hm_plot(args[2]);
	}
	else if (args[1] == "coverage")
	{
		check(4,true,argc,args);
		a.multiplicity(args[2], atoi(args[3].c_str()), false);
	}	
	else if (args[1] == "coverage_bs")
	{
		check(4,true,argc,args);
		a.multiplicity(args[2], atoi(args[3].c_str()), true);
	}	
	else
	{
		help();
	}
	return 0;
}
