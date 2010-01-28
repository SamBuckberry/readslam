#include "../tools/_preprocess.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Usage: ./preprocess path/to/infile.fastq path/to/outfile.fastq" << endl;
		exit(0);
	}
	ReadSlam::PreProcessor processor;
	processor.trim(argv[1], argv[2]);
}