#include "../tools/_postprocess.h"

int main (int argc, char * const argv[])
{
	if (argc != 5)
	{
		cout << "Usage: ./postprocess original.fastq bowtieF bowtieR outfile" << endl;
		exit(0);
	}
	ReadSlam::PostProcessor processor;
	processor.process(argv[1], argv[2], argv[3], argv[4]);
}