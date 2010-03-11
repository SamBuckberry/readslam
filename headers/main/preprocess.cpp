#include "../tools/_preprocess.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./preprocess adapter infile outfile" << endl;
		cout << "Example: ./preprocess AGATCGGAAGAGCTCGTATGC /tmp/reads.fastq /tmp/trimmed.fastq" << endl;
		exit(0);
	}
	ReadSlam::PreProcessor processor;
	processor.trim(argv[1], argv[2], argv[3]);
}