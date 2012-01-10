#include "../tools/_hmcip_resolve.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./trim ./genome.fasta ./infile.slam 150" << endl;
		return 1;
	}
	ReadSlam::Trimmer trimmer;
	trimmer.load(argv[1]);
	trimmer.process(argv[2], atoi(argv[3]));

	return 0;
}