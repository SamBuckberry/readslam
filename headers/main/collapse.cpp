#include "../tools/_collapser.h"

int main (int argc, char * const argv[])
{
	if (argc != 3)
	{
		cout << "Take a file of mapped .slam reads and collapse to remove clones." << endl;
		cout << "Keeps read with the highest net quality at each collapse site." << endl;
		cout << "NOTE: reads must be sorted first using sort_clonal" << endl;
		cout << "Usage: ./collapse ./uncollapsed.slam ./collapsed.slam" << endl;
		exit(0);
	}
	ReadSlam::Collapser::collapse_clones(argv[1], argv[2]);
}