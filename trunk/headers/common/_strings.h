#ifndef _STRINGS
#define _STRINGS

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace Strings
{
	//Echo a message to screen
	static void echo(string msg)
	{
		cout << msg << endl;
	}
	
	//Extract a substring from a string denoted by two boundary strings (takes the first value)
	static string between(string& source, string left, string right)
	{
		int start = 0;
		int end = source.size()-1;
		
		//Search for left target
		if (left != "")
		{
			size_t found = source.find(left);
			start = (found == string::npos) ? 0 : found + left.size();
		}

		//Search for right target
		if (right != "")
		{
			size_t found = source.find(right,start);
			end = (found == string::npos) ? source.size()-1 : found-1;
		}
		return source.substr(start, end-start+1);
	}
	
	//Replace all target substrings within a line with a new string
	static void replace(string& line, string target, string replacement)
	{
		size_t found = line.find(target);
	
		if (found != string::npos)
		{
			line.replace(found, target.size(), replacement);
		}
	}	

	//Reverse a string
	static string reverse(string line)
	{
		string rev = line;
		int i, len = line.size();
		
		for (i=0; i<len; i++)
		{
			rev[i] = line[len-1-i];
		}
		return rev;
	}
	
	//Trim a character off the left side of a string
	static string ltrim(string& s, const char c)
	{		
		for (int i=0,len=s.size(); i<len; ++i)
		{
			if (s[i] != c)
			{
				return s.substr(i);
			}
		}
		return "";
	}
	
	//Trim a string to remove whitespace from the ends
	static void trim(string& str)
	{
		string::size_type pos;
		
		//Erase off the back
		pos = str.find_last_not_of(" \r\n");
	
		if (pos != string::npos)
		{
			str.erase(pos + 1);
		}
	
		//Erase off the front
		pos = str.find_first_not_of(" \r\n");
	
		if (pos != string::npos)
		{
			str.erase(0, pos);
		}
	}
	
	//Explode a string by separator
	static int explode(const string& line, char sep, vector<string>& fields)
	{
		fields.clear();
		int len = line.size();
		if (len == 0) return 0;
		
		string field = "";
		
		for (int i=0; i<len; i++)
		{
			if (line[i] == sep)
			{
				fields.push_back(field);
				field = "";
				continue;
			}
			field += line[i];
		}
		fields.push_back(field);
		return fields.size();
	}
	
	//Implode a vector of strings to a single string
	static string implode(vector<string> fields, char sep)
	{
		int i, len = fields.size();
		string out;
		out.clear();
		
		if (len == 0) return out;
		
		out += fields[0];
		
		if (len == 1) return out;
		
		for (i=1; i<len; i++)
		{
			out += sep + fields[i];
		}
		return out;
	}
	
	//Split a line by a specified character
	static vector<string> split(const string& line, char c)
	{
		vector<string> fields;
		fields.clear();
		int last = 0;
		
		for (int i=0, len=line.size(); i<len; ++i)
		{
			if (line[i] == c)
			{
				fields.push_back(line.substr(last, i-last));
				last = i+1;
				continue;
			}
		}
		fields.push_back(line.substr(last, line.size() - last));
		
		return fields;
	}
	
	//Split a tab delimited line
	static vector<string> tabsplit(const string& line)
	{
		return Strings::split(line, '\t');
	}
		
	//Append a number to a string
	static string add_int(string base, int num)
	{
		ostringstream s;
		s << base << num;
		return s.str();
	}
	
	//Count the number of substrings in another string
	static unsigned long substring_count(const string& haystack, const string& needle)
	{
		unsigned long haylen = haystack.size();
		unsigned long neelen = needle.size();
		unsigned long tally = 0;
		
		for (unsigned long i=0; i<haylen - neelen; ++i)
		{
			for (unsigned long j=0; j<neelen; ++j)
			{
				if (needle[j] != haystack[i+j]) break;
				if (j == neelen - 1) ++tally;
			}
		}
		return tally;
	}
}

#endif
