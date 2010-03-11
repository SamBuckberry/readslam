#include "burrows.h"

int main(int argc, char * argv[])
{
	BurrowsWheeler burrows;
	burrows.file_encode(argv[1], argv[2]);
}
