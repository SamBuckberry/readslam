#include "../tools/_hmcip_resolve.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Generate statistics for mCG and mCH sites in mapped reads." << endl;
		cout << "If the specified read length is shorter than an actual read, the actual read length is used" << endl;
		cout << endl;
		cout << "Usage: ./hmcip_resolve ./genome.fasta ./reads.slam read_length" << endl;
		cout << "Example: ./hmcip_resolve ./genome.fasta ./infile.slam 150" << endl;
		return 1;
	}
	ReadSlam::HMC_Resolver resolver;
	resolver.load(argv[1]);
	resolver.process(argv[2], atoi(argv[3]));

	return 0;
}