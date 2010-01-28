#include "../tools/_sorter.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Sorts a file of .slam reads by location (assembly low to high, then coordinate)" << endl;
		cout << "Usage: ./sort_sequence ./unsorted.slam ./sorted.slam" << endl;
		exit(0);
	}
	ReadSlam::Sorter::sort_reads("location", 1000000, argv[1], argv[2]);
}