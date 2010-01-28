#include "../core/_genome2.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./map ./genome.fasta ./infile.slam ./outfile.slam" << endl;
		return 1;
	}
	//ReadSlam::Mapper mapper;
	ReadSlam::Genome genome;
	genome.load(argv[1]);

	return 0;
}