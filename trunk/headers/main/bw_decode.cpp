#include "../algorithms/burrows.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Decode a file using the Burrows-Wheeler algorithm." << endl;
		cout << "Usage: ./bw_decode /infile /outfile" << endl;
		exit(0);
	}
	BurrowsWheeler bw;
	
	bw.file_decode(argv[1], argv[2]);
}
