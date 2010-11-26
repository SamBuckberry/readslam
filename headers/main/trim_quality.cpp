#include "../tools/_trimmer.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./trim_quality quality_threshold infile outfile" << endl;
		cout << "Example: ./trim_quality 30 ./infile.slam ./outfile.slam" << endl;
		return 1;
	}
	ReadSlam::Trimmer trimmer;
	trimmer.trim_quality(atoi(argv[1]), argv[2], argv[3]);

	return 0;
}
