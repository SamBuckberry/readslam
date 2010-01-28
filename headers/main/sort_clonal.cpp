#include "../tools/_sorter.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Sorts a file of .slam reads by clone start (assembly low to high, then strand then native start site)" << endl;
		cout << "Usage: ./sort_sequence ./unsorted.slam ./sorted.slam" << endl;
		exit(0);
	}
	ReadSlam::Sorter::sort_reads("clonal", 1000000, argv[1], argv[2]);
}