#include "../algorithms/sorting.h"
#include "../tools/_fasta.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Encode a genome's fasta sequences using the Burrows-Wheeler algorithm." << endl;
		cout << "Usage: ./burrows_wheeler /infile.fa /outfile.fa" << endl;
		exit(0);
	}
	ReadSlam::FastA::Genome g;
	g.bw_encode(argv[1], argv[2]);
}
