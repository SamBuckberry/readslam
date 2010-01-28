#include "../tools/_trimmer.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./trim ./genome.fasta ./infile.slam ./outfile.slam" << endl;
		return 1;
	}
	ReadSlam::Trimmer trimmer;
	trimmer.load(argv[1]);
	trimmer.trim(argv[2], argv[3]);

	return 0;
}