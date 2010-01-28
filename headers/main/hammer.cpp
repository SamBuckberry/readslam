#include "../tools/_hammer.h"

int main (int argc, char * const argv[])
{
	if (argc != 6)
	{
		cout << "Stacks and calls reads assuming bisulfite treatment" << endl;
		cout << "Usage: ./hammer ./genome.fasta ./infile.slam ./outfile.stacks control falsepos" << endl;
		cout << "Example: ./hammer ./human.fa ./reads.slam ./stacks L 0.05" << endl;
		return 1;
	}
	ReadSlam::Hammer hammer;
	hammer.hammer(argv[1], argv[2], argv[3], argv[4], atof(argv[5]));
	return 0;
}