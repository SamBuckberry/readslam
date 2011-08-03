#include "../tools/_preprocess.h"

int main (int argc, char * const argv[])
{
	if (argc != 5)
	{
		cout << "Usage: ./preprocess adapter threshold infile outfile" << endl;
		cout << "Example: ./preprocess AGATCGGAAGAGCTCGTATGC 16 /tmp/reads.fastq /tmp/trimmed.fastq" << endl;
		exit(0);
	}
	ReadSlam::PreProcessor processor;
	processor.trim(argv[1], (char)atoi(argv[2]), argv[3], argv[4]);
}