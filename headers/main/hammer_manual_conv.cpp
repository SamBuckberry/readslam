#include "../tools/_hammer_manual_conv.h"

int main (int argc, char * const argv[])
{
	if (argc != 6)
	{
		cout << "Stacks and calls reads assuming bisulfite treatment. Uses a fixed failed conversion rate" << endl;
		cout << "Usage: ./hammer_manual_conv ./genome.fasta ./infile.slam ./outfile.stacks failure_rate falsepos" << endl;
		cout << "Example: ./hammer_manual_conv ./human.fa ./reads.slam ./stacks 0.03 0.05" << endl;
		return 1;
	}
	ReadSlam::HammerStatic hammer;
	hammer.hammer(argv[1], argv[2], argv[3], atof(argv[4]), atof(argv[5]));
	return 0;
}