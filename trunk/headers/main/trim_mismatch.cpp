#include "../tools/_trimmer.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Trim the 3' end of a read when > n mismatches are found" << endl;
		cout << "Usage: ./trim_mismatch genome mismatch_threshold reads_in reads_out" << endl;
		cout << "Example: ./trim_mismatch ./genome.fasta 2 ./infile.slam ./outfile.slam" << endl;
		return 1;
	}
	ReadSlam::Trimmer trimmer;
	trimmer.load_genome(argv[1]);
	trimmer.trim_mismatches(atoi(argv[2]), argv[3], argv[4]);

	return 0;
}