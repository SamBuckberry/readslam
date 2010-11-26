#include "../tools/_collapser.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Collapse a file of mapped .slam reads to remove clones." << endl;
		cout << "Keeps read with the highest net quality at each clone site." << endl;
		cout << "Usage: ./collapse_clones ./uncollapsed.slam ./collapsed.slam" << endl;
		exit(0);
	}
	ReadSlam::Collapser::collapse_clones(argv[1], argv[2]);
}