#include "../tools/_preprocess.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Convert Cs to Ts in all reads" << endl;
		cout << "Params: infile, outfile" << endl;
		cout << "Example: ./bisulfite_treat ./infile.slam ./outfile.slam" << endl;
		exit(0);
	}
	ReadSlam::PreProcessor processor;
	processor.trim(argv[1], argv[2]);
}