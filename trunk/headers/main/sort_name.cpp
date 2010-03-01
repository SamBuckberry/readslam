#include "../tools/_sorter.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Sorts a file of .slam reads by read name (id - alphabetical order)" << endl;
		cout << "Usage: ./sort_name ./unsorted.slam ./sorted.slam" << endl;
		exit(0);
	}
	ReadSlam::Sorter::sort_reads("name", 1000000, argv[1], argv[2]);
}