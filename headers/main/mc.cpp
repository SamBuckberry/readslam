#include "../tools/_mc.h"

int main (int argc, char * const argv[])
{
	if (argc != 4)
	{
		cout << "Usage: ./mc ./genome.fasta ./infile.stacks ./outfile.mc" << endl;
		return 1;
	}
	ReadSlam::MC::Builder builder;
	builder.load_genome(argv[1]);
	builder.build(argv[2], argv[3]);

	return 0;
}