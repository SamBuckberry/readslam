#include "../tools/_parser.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Usage: ./final2slam ./infile.final ./outfile.slam" << endl;
		exit(0);
	}
	ReadSlam::Parser::parse("final2slam", argv[1], argv[2]);
}