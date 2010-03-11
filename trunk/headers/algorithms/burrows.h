/*
 * Provides an encoder and decoder that uses the Burrows-Wheeler transformation
 * Also provides a searchable index (full-text search of a BWT string)
 *
 * Julian Tonti-Filippini (2010) julian.tonti@gmail.com
 */

/*			
   plantenergy       sorted        A         B
-----------------------------------------------
 0 plantenergy     antenergypl     a.........l
 1 lantenergyp     energyplant     e.........t
 2 antenergypl     ergyplanten     e.........n
 3 ntenergypla     gyplantener     g.........r
 4 tenergyplan     lantenergyp     l.........p
 5 energyplant     nergyplante     n.........e
 6 nergyplante     ntenergypla     n.........a
 7 ergyplanten     plantenergy*    p.........y* <-seed = 7
 8 rgyplantene     rgyplantene     r.........e
 9 gyplantener     tenergyplan     t.........n
10 yplantenerg     yplantenerg     y.........g
                                   -----------
                                   ltnrpeayeng
*/
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "sorting.h"

using namespace std;

/*
 * Note: columnA and the links vector can both be compressed drastically to reduce
 * their memory footprints. The cost is speed as [n] lookup operations require
 * log(n) instead of (1) due to binary search
 */
struct BurrowsWheeler
{
	static const int LETTERS = 256;
	
	//Used in the compression of columnA
	struct Range { int x1,x2; };
		
	//Comparator object for sorting sequence blocks
	struct Comparator
	{
		//These are required because std::sort requires a static comparator
		string seq;
		int    len;

		bool operator() (const int a, const int b)
		{
			if (a == b) return false;

			int posA = a;
			int posB = b;

			while (seq[posA] == seq[posB])
			{
				if (++posA == len) posA = 0;
				if (++posB == len) posB = 0;
				if (posA == a) return false;
			}
			return seq[posA] < seq[posB];
		}			
	} comparator;	

	//Instance variables
	vector<Range> groups; //Ranges for each character in columnA
	vector<int>    links; //Follow this through to trace the original sequence, a list of pointers to the index of the next block (blockstarts)
	vector<int>  mapback; //Provides a map from the ordered blocks to the original sequence position
	string       columnA; //The columnA sequence in the sorted matrix
	string       columnB; //The columnB sequence in the sorted matrix
	string      original; //The original sequence
	int           length; //Length of the string (number of blocks)
	int             seed; //The block-index of the original start position
	
	//Build columnA from any string (internally sorted string)
	private: void buildA(string &str)
	{
		//Count occurrences of each character
		vector<int> counts(LETTERS,0);

		for (int i=0; i<length; ++i)
		{
			counts[str[i]]++;
		}

		//Use the counts to store the range of coverage for each character
		groups.resize(LETTERS);
		
		for (int i=0, pos=0; i<LETTERS; ++i)
		{
			if (counts[i] == 0)
			{
				groups[i].x1 = -1;
				groups[i].x2 = -1;
				continue;
			}
			groups[i].x1 = pos;
			groups[i].x2 = pos + counts[i] - 1;
			
			//Populate the characters of columnA for this range
			for (int j=groups[i].x1; j<=groups[i].x2; ++j)
			{
				columnA[j] = (char)i;
			}

			//Jump to the start of the next range
			pos += counts[i];
		}
	}
	
	//Build columnB (the encoded string) from a raw string (assumes A is already built)
	private: void buildB(string &str)
	{
		//Build the sequence-block table
		vector<int> blockstarts(length,0);

		for (int i=0; i<length; ++i)
		{
			blockstarts[i] = i;
		}

		//Sort the sequence blocks by alphabetical order
		comparator.seq = str;
		comparator.len = length;
		
		//BlockSort sorter;
		BZSort sorter;
		sorter.sort(str, blockstarts);		
		//sort(blockstarts.begin(), blockstarts.end(), comparator);
		
		//Build columnB (the encoding)
		for (int i=0; i<length; ++i)
		{
			//The 'blockend' value is simply blockstart - 1, wrapping if blockstart == 0
			int blockstart = blockstarts[i];
			int blockend   = blockstart - 1;
					
			//Store the seed value when encountered
			if (blockstart == 0)
			{
				blockend = length - 1;
				seed = i;
			}
			columnB[i] = str[blockend];
		}
	}
	
	//Build the index. Assumes that columns A and B are already built, and that the seed is known
	private: void build_index()
	{
		//Counters for each of the characters in the sequence
		vector<int> counters(LETTERS,0);
		
		//For each position in the encoded string (each of the blocks in the currently sorted matrix)
		links.resize(length,-1);
		
		for (int i=0; i<length; ++i)
		{
			//The character for columnB (known from the encoded string) will become 'new_a' when we move it to the front
			int old_a = (unsigned char)columnA[i];
			int new_a = (unsigned char)columnB[i];
			
			//Now we are effectively moving charB to the front (making it charA) and re-sorting to determine the new block index (offset[charB] + counters[charB])
			int old_index = i;
			int new_index = groups[new_a].x1 + counters[new_a];
			
			//We know that new_a comes before old_a in the sequence, so we know that new_index comes before old_index. Mapping each of these into columnA gives
			//us the two consecutive characters.
			links[new_index] = old_index;
			
			//Increment the counter for the processed character
			counters[new_a]++;
		}
		
		//Build the vector that maps M back to the original string
		mapback.resize(length,-1);
		
		for (int i=0, pos=seed; i<length; ++i)
		{
			mapback[pos] = i;
			pos = links[pos];
		}
	}
	
	//Take a raw string and encode it
	public: void encode(string &decoded)
	{
		if (decoded.size() == 0) return;
		
		original = decoded;
		length = original.size();
		columnA.resize(length);
		columnB.resize(length);
		
		buildA(original);
		buildB(original);
		build_index();
	}
		
	//Decode an encoded string
	public: void decode(string &encoded, int start)
	{
		if (encoded.size() == 0) return;
		
		length = encoded.size();
		columnA.resize(length);
		columnB.resize(length);
		original.resize(length);
		columnB = encoded;
		seed = start;
		
		buildA(encoded);
		build_index();
		decode_substring(original,seed,length);
	}
	
	//Encode a file
	public: void file_encode(string infile, string outfile)
	{
		ifstream in (infile.c_str());

		char c;
		string seq = "";
		
		while (true)
		{
			in.get(c);
			if (!in.good()) break;
			seq += c;
		}
		in.close();
		seq.resize(seq.size() - 1); //trim off the last newline
		
		encode(seq);

		ofstream out(outfile.c_str());
		out << seed << "\n" << columnB;
		out.close();
	}
	
	//Decode a file
	public: void file_decode(string infile, string outfile)
	{
		string line = "";
		string seq = "";
		
		ifstream in(infile.c_str());
		
		//The seed is the first line
		getline(in, line);
		seed = atoi(line.c_str());
		
		char c;
		
		while (in.good())
		{
			in.get(c);
			seq += c;
		}
		in.close();
		seq.resize(seq.size() - 1); //trim off the last newline		

		decode(seq, seed);

		ofstream out(outfile.c_str());
		out << original << endl;
		out.close();
	}
	
	//Decode a substring
	public: void decode_substring(string &out, int pos, int n)
	{
		out.resize(n);
		
		for (int i=0; i<n; ++i)
		{
			out[i] = columnA[pos];
			pos = links[pos];
		}
	}
	
	//Matches the pattern from right to left (it's possible to search from left to right if the original input and output are reversed)
	public: Range find(string &pattern)
	{
		//The current result range (pruned to fit into range z)
		Range y = {-1,-1};
		
		//The previous result range, starting with all rows
		Range z = {0,length};
		
		for (int i=pattern.size()-1; i>=0; --i)
		{
			//The range of rows that start with Y
			y = groups[pattern[i]];
			
			//Bomb immediately if Y rows can't possibly link into Z rows that are contained in Z (also bombs if Y is empty)
			if (links[y.x1] > z.x2 || links[y.x2] < z.x1)
			{
				y.x1 = -1;
				y.x2 = -1;
				break;
			}
			
			//Prune Y so that it only contains rows that link into Z (loop is simple but inefficient, binary search is better)
			//while (links[y.x1] < z.x1) y.x1++;
			//while (links[y.x2] > z.x2) y.x2--;
			if (z.x1 > links[y.x1]) y.x1 = search_links(y, z.x1);
			if (z.x2 < links[y.x2]) y.x2 = search_links(y, z.x2);

			//The intersect-adjusted Y now becomes Z
			z = y;
		}
		return y;
	}
	
	//Binary search of a subrange of the links vector for a specific value. Assumes sanity checking in advance (value MUST be in list)
	private: int search_links(Range r, int target)
	{
		int m = 0;

		while (r.x1 <= r.x2)
		{
			m = (r.x1 + r.x2) / 2;
			if      (target < links[m]) r.x2 = m - 1;
			else if (target > links[m]) r.x1 = m + 1;
			else    break;
		}
		return m;
	}
	
	//Take an index from the sorted blocks and return the original string position
	public: int index2pos(int idx)
	{
		return mapback[idx];
	}
		
	public: void print_links()
	{
		for (int i=0; i<length; ++i)
		{
			if (links[i] >= 0)
			{
				cout << i << " -> " << links[i] << endl;
			}
		}
	}
	
	public: void print_matrix()
	{
		string s;
		
		for (int i=0; i<length; ++i)
		{
			decode_substring(s,i,length);
			cout << s << " :" << i << endl;
		}
	}
	
	//TODO: loading and saving with compression steps
	void load(string infile)  { cerr << "BurrowsWheeler::load not implemented yet" << endl; }
	void save(string outfile) { cerr << "BurrowsWheeler::save not implemented yet" << endl; }
};
