#include "../tools/_splitter.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Usage: ./split ./infile.reads ./outfile.reads" << endl;
		exit(0);
	}
	ReadSlam::Splitter::split(argv[1], argv[2]);
}