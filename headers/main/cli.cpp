/*
 * C++ based management utility for ReadSlam
 *
 * (C) 2009 Julian Tonti-Filippini
 * julian.tonti@gmail.com
 *
 * Disclaimer: you use this entirely at your own risk
 */
#include "../tools/_cli.h"

void bomb(string msg)
{
	cerr << msg << endl;
	exit(1);
}

void help_quick()
{
	cout << "\n"
	<< "\nCommands:"
	<< "\n    HELP"
	<< "\n    TRIM in.fastq out.fastq"
	<< "\n    MAP genome.fasta in.fastq out.reads seedsize"
	<< "\n    MAPBS genome.fasta in.fastq out.reads seedsize"
	<< "\n    LIST_MAP genome.fasta in.fastq out.reads seedsize"
	<< "\n    LIST_MAPBS genome.fasta in.fastq out.reads seedsize"
	<< "\n    STACK genome.fasta in.reads out.stacks"
	<< "\n    CALL genome.fasta in.reads out.calls"
	<< "\n    PARSE type infile outfile"
	<< "\n    SORT type chunksize infile outfile"
	<< "\n"
	<< endl;
	exit(0);
}

void help_full()
{
	cout << "\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: ReadSlam ::"
	<<"\n"
	<<"\nThis utility provides a collection of routines to take FastQ data through to AJ."
	<<"\n  - TRIM       : remove adapters and low quality 3' sequence"
	<<"\n  - MAP        : align reads to a reference genome (normal DNA)"
	<<"\n  - MAPBS      : align reads to a reference genome (NaBS treated DNA)"
	<<"\n  - LIST_MAP   : same as MAP with a different indexing system"
	<<"\n  - LIST_MAPBS : same as MAPBS with a different indexing system"
	<<"\n  - STACK      : stack aligned reads against a reference genome"
	<<"\n  - CALL       : identify methylation sites"
	<<"\n  - PARSE      : convert between filetypes"
	<<"\n  - SORT       : sort the reads in a file"
	<<"\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: Examples ::"
	<<"\nNote: command keywords are case sensitive"
	<<"\n"
	<<"\n- Show this help information"
	<<"\n    ./readslam HELP"
	<<"\n    ./readslam"
	<<"\n"
	<<"\n- Trim reads to remove adapter sequence and low quality 3' calls"
	<<"\n    ./readslam TRIM ./reads.fastq ./trimmed.fastq"
	<<"\n"
	<<"\n- Map reads against a reference genome using a <vector> index"
	<<"\n    ./readslam MAP ./human.fasta ./trimmed.fastq ./reads 12"
	<<"\n    ./readslam MAPBS ./human.fasta ./trimmed.fastq ./reads 12"
	<<"\n"
	<<"\n- Map reads against a reference genome using a <map> index"
	<<"\n    ./readslam LIST_MAP ./human.fasta ./trimmed.fastq ./reads 12"
	<<"\n    ./readslam LIST_MAPBS ./human.fasta ./trimmed.fastq ./reads 12"
	<<"\n"
	<<"\n- Stack reads against a reference genome"
	<<"\n    ./readslam STACK ./human.fasta ./reads ./stacks"
	<<"\n"
	<<"\n- Make methylation calls"
	<<"\n    ./readslam CALL ./human.fasta ./reads ./calls"
	<<"\n"
	<<"\n- Parse between file types"
	<<"\n    ./readslam PARSE slam2raw ./reads.slam ./reads.raw"
	<<"\n    ./readslam PARSE slam2fastq ./reads.slam ./reads.fastq"
	<<"\n    ./readslam PARSE slam2annoj ./reads.slam ./reads.annoj"
	<<"\n    ./readslam PARSE slam2bowtie ./reads.slam ./reads.bowtie"
	<<"\n"
	<<"\n    ./readslam PARSE raw2slam ./read.raw ./reads.slam"
	<<"\n    ./readslam PARSE fastq2slam ./read.fastq ./reads.slam"
	<<"\n    ./readslam PARSE annoj2slam ./read.annoj ./reads.slam"
	<<"\n    ./readslam PARSE bowtie2slam ./read.bowtie ./reads.slam"
	<<"\n"
	<<"\n- Sort the reads in a file"
	<<"\n    ./readslam SORT sequence 1000000 ./reads.random ./reads.sorted"
	<<"\n    ./readslam SORT location 1000000 ./reads.random ./reads.sorted"
	<<"\n    ./readslam SORT clone 1000000 ./reads.random ./reads.sorted"
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

int main (int argc, char* const argv[])
{
	vector<string> args;
	
	for (int i=0; i<argc; i++)
	{
		args.push_back(string(argv[i]));
	}
	
	if (argc == 1)
	{
		help_quick();
	}
	
	if (args[1] == "HELP")
	{
		help_full();
	}
	
	ReadSlam::CLI handler;
	
	if (args[1] == "TRIM")
	{
		if (argc != 4) bomb("Incorrect parameter count for TRIM");
		handler.trim(args[2], args[3]);
	}
	else if (args[1] == "MAP")
	{
		if (argc != 6) bomb("Incorrect parameter count for MAP");
		handler.map(args[2], args[3], args[4], atoi(args[5].c_str()), false, false);
	}
	else if (args[1] == "MAPBS")
	{
		if (argc != 6) bomb("Incorrect parameter count for MAP");
		handler.map(args[2], args[3], args[4], atoi(args[5].c_str()), true, false);
	}
	else if (args[1] == "LIST_MAP")
	{
		if (argc != 6) bomb("Incorrect parameter count for MAP");
		handler.map(args[2], args[3], args[4], atoi(args[5].c_str()), false, true);
	}
	else if (args[1] == "LIST_MAPBS")
	{
		if (argc != 6) bomb("Incorrect parameter count for MAP");
		handler.map(args[2], args[3], args[4], atoi(args[5].c_str()), true, true);
	}	
	else if (args[1] == "STACK")
	{
		if (argc != 5) bomb("Incorrect parameter count for STACK");
		handler.stack(args[2], args[3], args[4]);
	}
	else if (args[1] == "METH")
	{
		if (argc != 5) bomb("Incorrect parameter count for METH");
		handler.meth(args[2], args[3], args[4]);
	}
	else if (args[1] == "PARSE")
	{
		if (argc != 5) bomb("Incorrect parameter count for PARSE");
		handler.parse(args[2], args[3], args[4]);
	}
	else if (args[1] == "SORT")
	{
		if (argc != 6) bomb("Incorrect parameter count for SORT");
		handler.sort_reads(args[2], atoi(args[3].c_str()), args[4], args[5]);
	}
	else
	{
		bomb("Unknown command. Legal values are HELP | TRIM | MAP | MAPBS | STACK | METH | PARSE | SORT");
	}
	return 0;
}
