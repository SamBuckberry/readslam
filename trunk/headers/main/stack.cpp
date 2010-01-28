#include "../tools/_stacker.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./stack ./genome.fasta ./infile.slam ./outfile.stacks" << endl;
		return 1;
	}
	ReadSlam::Stacker stacker;
	stacker.load_genome(argv[1]);
	stacker.stack(argv[2]);
	stacker.call();
	stacker.save(argv[3]);

	return 0;
}