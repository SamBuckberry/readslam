#include "../tools/_stacker.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./split_stacks ./genome.fasta ./infile.stacks ./outfile.stacks" << endl;
		return 1;
	}
	ReadSlam::Stacker stacker;
	stacker.load_genome(argv[1]);
	stacker.load(argv[2]);
	stacker.split(argv[3]);

	return 0;
}