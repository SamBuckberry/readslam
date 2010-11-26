#include "../tools/_preprocess.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Reinstate converted Cs back to Ts in mapped reads" << endl;
		cout << "Params: genome, mapped_reads, outfile" << endl;
		cout << "Example: ./bisulfite_revert ./genome.fasta ./infile.slam ./outfile.slam" << endl;
		exit(0);
	}
	ReadSlam::PreProcessor processor;
	processor.trim(argv[1], argv[2]);
}