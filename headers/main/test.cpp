//#include "_collapse.h"
#include "_files.h"
#include "_sorter.h"

bool cmp(const string& a, const string& b)
{
	return b < a;
}
int main (int argc, char * const argv[])
{
	//ReadSlam::Collapser c;
	//c.collapse(argv[1], argv[2]);
	
	//Unit tests for file functions
	// ofstream out("./test_in");
	// out << "1" << endl;
	// out << "2" << endl;
	// out << "aaa" << endl;
	// out << "aa" << endl;
	// out << "a" << endl;
	// out << "AA" << endl;
	// out << "-" << endl;
	// out << "" << endl;
	// out << " " << endl;
	// out << " a" << endl;
	// out << "-2.4" << endl;
	// out << "10000" << endl;
	// out << "\n" << endl;
	// out << "\n" << endl;
	// out << "asdf" << endl;
	// out << "zz" << endl;
	// out.close();
	// 
	// //vector<string> filenames = Files::chunk_split("./test_in", "./test_out", 5);
	// //Files::concat("./merged", filenames);
	// //Files::ram_sort("./test_in", "./ram_sorted", cmp);
	// //Files::interleave_split("./test_in", filenames);
	// //Files::interleave_join(filenames, "./test_rejoined");
	// Files::disk_sort("./test_in", "./test_out", cmp, 3);
	
	
	//Unit test for sorter
	//id, assembly, strand, position, copies, locations, mismatches, score, sequence, qualities	
	ofstream out("./test");
	out << "1\tC\t+\t10000\t10\t1\t5\t100\tAAAAAAAAAA\taaaababbaa" << endl;
	out << "2\t1\t-\t10000\t10\t1\t5\t100\tAAAAAAAAAT\taaaababbaa" << endl;
	out << "3\t2\t+\t10000\t10\t1\t0\t110\tAAAAAAAAAG\taaaababbaa" << endl;
	out << "4\tM\t-\t106600\t1\t5\t1\t100\tAAAAAAAAAC\taaaababbaa" << endl;
	out << "5\tGH\t+\t15000\t20\t1\t5\t100\tAAAAATAAAA\taaaababbaa" << endl;
	out << "6\tC\t-\t20000\t10\t1\t0\t130\tAAAAAAGAAA\taaaababbaa" << endl;
	out << "7\tC\t+\t10000\t40\t3\t5\t100\tAAAAAACAAA\taaaababbaa" << endl;
	out << "a\tC\t+\t10000\t10\t1\t0\t150\tAAATAAAAAA\taaaababbaa" << endl;
	out << "aaa\tC\t+\t10000\t10\t1\t5\t100\tACAAAAAAAA\taaaababbaa" << endl;
	out.close();
	
	//ReadSlam::Sorter s;
	//s.test("./test");
	//s.sort_reads("./test", "./out1", "sequence");
	//s.sort_reads("./test", "./out2", "clones");
	//s.sort_reads("./test", "./out3", "position");
	
}