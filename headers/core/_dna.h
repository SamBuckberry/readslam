#ifndef _READSLAM_DNA
#define _READSLAM_DNA

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

//Static functions
namespace DNA
{
	//Clean a DNA string
	void clean (string& dna)
	{
		for (int i=0, len=dna.size(); i<len; ++i)
		{
			switch (dna[i])
			{
				case 'A' : break;
				case 'T' : break;
				case 'C' : break;
				case 'G' : break;
				case 'a' : dna[i] = 'A'; break;
				case 't' : dna[i] = 'T'; break;
				case 'c' : dna[i] = 'C'; break;
				case 'g' : dna[i] = 'G'; break;
				default  : dna[i] = 'N';
			}
		}
	}
	
	//Reverse a DNA string
	string reverse (const string& dna)
	{
		string out = dna;
		
		for (int i=0, len=out.size(); i<len; ++i)
		{
			out[i] = dna[len-1-i];
		}
		return out;
	}
	
	//Complement a DNA string
	string complement (const string& dna)
	{
		string out = dna;

		for (int i=0, len=out.size(); i<len; ++i)
		{
			switch (dna[i])
			{
				case 'A' : out[i] = 'T'; break;
				case 'T' : out[i] = 'A'; break;
				case 'C' : out[i] = 'G'; break;
				case 'G' : out[i] = 'C'; break;
				default  : out[i] = 'N';
			}
		}
		return out;
	}
	
	//Reverse complement a DNA string
	string reverse_complement (const string& dna)
	{
		string out = dna;

		for (int i=0, len=out.size(); i<len; ++i)
		{
			switch (dna[len-1-i])
			{
				case 'A' : out[i] = 'T'; break;
				case 'T' : out[i] = 'A'; break;
				case 'C' : out[i] = 'G'; break;
				case 'G' : out[i] = 'C'; break;
				default  : out[i] = 'N';
			}
		}
		return out;
	}
	
	//Convert a sequence to a bisulfite sequence
	string bisulfite (const string& dna)
	{
		string out = dna;

		for (int i=0, len=out.size(); i<len; ++i)
		{
			if (out[i] == 'C')
			{
				out[i] = 'T';
			}
		}
		return out;
	}
	
	//Generate indices for a sequence
	void seq2indices(string& seq, vector<int>& indices, int seed, bool bs)
	{
		if (seed > 15)
		{
			cerr << "Seed size cannot exceed 15" << endl;
			exit(1);
		}
		int base = 4;
		int mask = (int)(pow((double)base,(double)seed) - 1);
		int length = seq.size();

		indices.clear();
		indices.resize(length, -1);
		
		int index = 0;
		int nbits = 0;
		
		for (int i=0; i<length; i++)
		{
			switch (seq[i])
			{
				case 'A' : index <<= 2; break;
				case 'C' : 
					if (!bs)
					{
						index <<= 2; index++;
					}
					else
					{
						index <<= 1; index++; index <<= 1; index++;
					}
				break;
				case 'G' : index <<= 1; index++; index <<= 1; break;
				case 'T' : index <<= 1; index++; index <<= 1; index++; break;
				default : nbits = 0; index = 0; continue;
			}
			if ((++nbits) == seed)
			{
				indices[i-(seed-1)] = index & mask;
				nbits--;
			}
		}
	}

/*
	//Provide all indices for a DNA sequence in bisulfite mode
	void seq2indicesBS(const char* seq, int length, vector<int>& indices, int seed)
	{
		int mask = (int)pow(2,seed) - 1;

		indices.resize(length, -1);
		
		int index = 0;
		int nbits = 0;
		
		for (int i=0; i<length; i++)
		{
			switch (seq[i])
			{
				case 'A' : index <<= 1; break;
				case 'G' : index <<= 1; break;
				case 'C' : index <<= 1; index++; break; 
				case 'T' : index <<= 1; index++; break;
				default  : nbits = 0; index = 0; continue;
			}
			if ((++nbits) == seed)
			{
				indices[i-(seed-1)] = index & mask;
				nbits--;
			}
		}
	}
*/
}
#endif