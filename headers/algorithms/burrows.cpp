/*
 * Provides an encoder and decoder that uses the Burrows-Wheeler transformation
 * Also provides a searchable index (full-text search of a BWT string)
 *
 * Julian Tonti-Filippini (2010) julian.tonti@gmail.com
 */
#include <iostream>
#include <vector>
#include <string>
#include "burrows.h"

using namespace std;

//Get a line of text from the user
string get_user_input()
{
	string s = "";
	
	while (true)
	{
		char c = getc(stdin);
		if (c == '\n') break;
		s.push_back(c);
	}
	return s;
}
	
int main(int argc, char * argv[])
{
	//Get the raw sequence from the user
	cout << "Enter a sequence to encode: " << flush;
	
	string entered = get_user_input();
	
	//Build a BWT to represent the string
	BurrowsWheeler bw;
	bw.encode(entered);
	string encoded = bw.columnB;
	
	bw.decode(encoded, bw.seed);
	string decoded = bw.original;
	
	//Print information for the user to see
	cout << "\nEntered:\n" << entered << endl;
	cout << "\nEncoded:\n" << encoded << endl;
	cout << "\nDecoded:\n" << decoded << endl;
	
	cout << "\nMatrix:" << endl;
	bw.print_matrix();
	
	cout << "\nLinks:" << endl;
	bw.print_links();
	
	//Ask the user for substrings and search for them in the index
	while (true)
	{
		cout << "\n\nSearch for substring: " << flush;
		
		char c = '\0';
		string pattern = get_user_input();
		
		if (pattern.size() == 0) break;
		
		BurrowsWheeler::Range range = bw.find(pattern);
		
		cout << "Result range is [" << range.x1 << "," << range.x2 << "]" << endl;
		
		if (range.x1 == -1)
		{
			cout << "0 hits found" << endl;
			continue;
		}
		cout << (range.x2 - range.x1 + 1) << " hits found\n" << endl;
		
		string s;
		
		while (range.x1 <= range.x2)
		{
			bw.decode_substring(s, range.x1, pattern.size() + 10);
			cout << bw.index2pos(range.x1) << ": " << s << "..." << endl;
			range.x1++;
		}
	}
}