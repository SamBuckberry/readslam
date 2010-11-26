#include "../tools/_trimmer.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Trims an adapter sequence off the back of a read" << endl;
		cout << "For best results, don't use the whole adapter sequence" << endl;
		cout << "Usage: ./trim_adapter adapter_sequence infile outfile" << endl;
		cout << "Example: ./trim_adapter AATATAGGGGCATACG ./infile.slam ./outfile.slam" << endl;
		return 1;
	}
	ReadSlam::Trimmer trimmer;
	trimmer.trim_adapter(argv[1], argv[2], argv[3]);

	return 0;
}
