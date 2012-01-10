#include "../tools/_preprocess_adapter_trim_only.h"

int main (int argc, char * const argv[])
{
	if (argc != 6)
	{
		cout << "Usage: ./preprocess adapter min_adapter_length qual_threshold infile outfile" << endl;
		cout << "Example: ./preprocess AGATCGGAAGAGCTCGTATGC 6 16 /tmp/reads.fastq /tmp/trimmed.fastq" << endl;
		exit(0);
	}
	ReadSlam::PreProcessor processor;
	processor.trim(argv[1], atoi(argv[2]), (char)atoi(argv[3]), argv[4], argv[5]);
}