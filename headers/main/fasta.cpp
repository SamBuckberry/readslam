/*
 * CLI for various FastA file manipulations
 */
#include "../tools/_fasta.h"

void bomb(string msg)
{
	cerr << msg << endl;
	exit(1);
}
void check(int num, bool exact, int argc, vector<string> args)
{
	if ((exact && argc != num) || argc < num)
	{
		cerr << "Error: bad parameter count for: " + args[1] << endl;
		exit(1);
	}
}
void help()
{
	cout << "\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: FastA ::"
	<<"\n"
	<<"\nA collection of utilities for working with FastA files"
	<<"\n  - reverse            : reverse sequences in FastA file"
	<<"\n  - complement         : complement sequences in FastA file"
	<<"\n  - reverse_complement : reverse complement sequences in FastA file"
	<<"\n  - bisulfite          : emulate NaBS treatment by converting Cs to Ts"
	<<"\n  - single             : convert sequences to single line format"
	<<"\n  - multi              : convert sequences to multi line format"
	<<"\n  - header_add         : add/replace the first header"
	<<"\n  - header_remove      : remove the first header"
	<<"\n  - get                : get a particular assembly and its sequence"
	<<"\n  - explode            : explode FastA file into many files, one per assembly"
	<<"\n  - implode            : implode many FastA files into one"
	<<"\n  - diagnose           : print assembly names and sizes"
	<<"\n"
	<<"\n--------------------------------------------------------------------------------"
	<<"\n"
	<<"\n:: Examples ::"
	<<"\nNote: command keywords are case sensitive"
	<<"\n"
	<<"\n- Show this help information"
	<<"\n    ./fasta"
	<<"\n"
	<<"\n- Sequence modification"
	<<"\n    ./fasta reverse ./old.fasta ./new.fasta"
	<<"\n    ./fasta complement ./old.fasta ./new.fasta"
	<<"\n    ./fasta reverse_complement ./old.fasta ./new.fasta"
	<<"\n    ./fasta bisulfite ./old.fasta ./new.fasta"
	<<"\n"
	<<"\n- Formatting"
	<<"\n    ./fasta single ./old.fasta ./new.fasta"
	<<"\n    ./fasta multi ./old.fasta ./new.fasta"
	<<"\n"
	<<"\n- Headers"
	<<"\n    ./fasta header_add header_without_gt ./old.fasta ./new.fasta"
	<<"\n    ./fasta header_remove ./old.fasta ./new.fasta"
	<<"\n"
	<<"\n- Construction / deconstruction"
	<<"\n    ./fasta get header_without_gt ./old.fasta ./new.fasta"
	<<"\n    ./fasta explode ./old.fasta ./new.fasta"
	<<"\n    ./fasta implode ./new.fasta ./old.fasta.1 ./old.fasta.2 ./old.fasta.3 ..."
	<<"\n"
	<<"\n- Diagnosis"
	<<"\n    ./fasta diagnose ./file.fasta"
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
		help();
	}
	
	ReadSlam::FastA::Genome g;

	if (args[1] == "reverse")
	{
		check(4,true,argc,args); g.reverse(args[2], args[3]);
	}
	else if (args[1] == "complement")
	{
		check(4,true,argc,args); g.complement(args[2], args[3]);
	}
	else if (args[1] == "reverse_complement")
	{
		check(4,true,argc,args); g.reverse_complement(args[2], args[3]);
	}
	else if (args[1] == "bisulfite")
	{
		check(4,true,argc,args); g.bisulfite(args[2], args[3]);
	}
	else if (args[1] == "single")
	{
		check(4,true,argc,args); g.single(args[2], args[3]);
	}	
	else if (args[1] == "multi")
	{
		check(4,true,argc,args); g.multi(args[2], args[3]);
	}
	else if (args[1] == "header_add")
	{
		check(5,true,argc,args); g.header_add(args[2], args[3], args[4]);
	}
	else if (args[1] == "header_remove")
	{
		check(4,true,argc,args); g.header_remove(args[2], args[3]);
	}
	else if (args[1] == "get")
	{
		check(5,true,argc,args); //g.get(args[2], args[3], args[4]);
	}
	else if (args[1] == "explode")
	{
		check(4,true,argc,args); g.explode(args[2], args[3]);
	}
	else if (args[1] == "implode")
	{
		vector<string> files;
		for (int i=3; i<argc; ++i)
		{
			files.push_back(args[i]);
		}
		check(4,false,argc,args); g.implode(files, args[2]);
	}
	else if (args[1] == "diagnose")
	{
		check(3,true,argc,args); g.diagnose(args[2]);
	}
	else
	{
		bomb("Error: unknown command: " + args[1]);
	}
	return 0;
}
