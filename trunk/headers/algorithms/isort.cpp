#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include "../common/_files.h"
#include "sorting.h"

using namespace std;

void dump(vector<int> &blocks, string &s)
{
	int len = blocks.size();
	// for (int i=0; i<len; ++i)
	// {
	// 	int a = blocks[i];
	// 	
	// 	for (int j=0; j<len; ++j)
	// 	{
	// 		int p = a+j >= len ? a+j-len : a+j;
	// 		cout << s[p];
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;
	for (int i=0, len=blocks.size(); i<len; ++i)
	{
		cout << s[blocks[i]];
	}
	cout << endl;
}

int main(int argc, char * argv[])
{
	string s = "qwertyuiopasdfghjklzxcvbnm";
	//s = "ertyu";
	//s = "aaaccaacgggacgagctggagcgatcgatgc";
	//s = "the cat sat on the mat and shat and I said to the cat, you are one dirty little brat";
	Files::slurp(argv[1],s); 
		
	vector<char> v;
	
	for (int i=0; i<s.size(); ++i)
	{
		v.push_back(s[i]);
	}
	cout << "Sorting" << endl;
	
	//QuickSort<char> sorter;
	BlockSort sorter;
	vector<int> blocks;
	sorter.sort(s,blocks);
	
	//dump(blocks,s);
}