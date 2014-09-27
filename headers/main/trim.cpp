#include "../tools/_trimmer.h"

int main (int argc, char * const argv[])
{
	if (argc != 7)
	{
		cout << "Usage: ./trim genome infile outfile max_mchs max_errs min_size" << endl;
		cout << endl;
		cout << "Param 1: (path) genome: full path to the reference genome FastA file" << endl;
		cout << "Param 2: (path) infile: full path to the input file of mapped reads in .slam format" << endl;
		cout << "Param 3: (path) outfile: full path to the output file (also .slam format)" << endl;
		cout << "Param 4: (+int) max_mchs: read kept if CH count <= max_mchs" << endl;
		cout << "Param 5: (+int) max_errs: read kept if mismatch count <= max_errs" << endl;
		cout << "Param 6: (+int) min_size: read kept if read length >= min_size" << endl;
		cout << endl;
		cout << "Example: ./trim ./human.fa ./reads.slam ./trimmed_reads.slam 4 3 20" << endl;		
		return 1;
	}
	ReadSlam::Trimmer trimmer;
	trimmer.load(argv[1]);
	trimmer.trim(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));

	return 0;
}

