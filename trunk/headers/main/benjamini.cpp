#include "../tools/_benjamini.h"

int main (int argc, char * const argv[])
{
	if (argc != 6)
	{
		cout << "Stacks and calls reads assuming bisulfite treatment" << endl;
		cout << "Usage: ./benjamini ./genome.fasta ./infile.slam ./outfile.stacks control falsepos" << endl;
		cout << "Example: ./benjamini ./human.fa ./reads.slam ./stacks L 0.05" << endl;
		return 1;
	}
	ReadSlam::Benjamini benjamini;
	benjamini.benjamini(argv[1], argv[2], argv[3], argv[4], atof(argv[5]));
	return 0;
}