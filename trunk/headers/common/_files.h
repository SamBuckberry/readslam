#ifndef _FILES
#define _FILES

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "_strings.h"

using namespace std;

namespace Files
{	
	//Split a file into an unknown number of files (determined dynamically by chunk size)
	static vector<string> chunk_split(string original, string outbase, int chunk)
	{
		vector<string> files;
		files.clear();
		
		ifstream in(original.c_str());
		ofstream out;

		string line = "";
		string file = "";
		
		while (in.good() && !in.eof())
		{
			file = Strings::add_int(outbase + ".", files.size());
			files.push_back(file);
			out.open(file.c_str());
			
			for (int i=0; i<chunk; ++i)
			{
				if (!getline(in,line)) break;
				out << line << endl;
			}
			out.close();
		}
		in.close();
		
		return files;
	}
		
	//Sort the lines of a file by loading it entirely into memory (suitable for small files)
	static void ram_sort(string infile, string outfile, bool (*comparator) (const string&, const string&))
	{
		vector<string> lines;
		lines.clear();
		
		ifstream in(infile.c_str());
		string line = "";
		
		while (getline(in,line))
		{
			lines.push_back(line);
		}
		in.close();
		
		if (comparator == NULL)
		{
			sort(lines.begin(), lines.end());
		}
		else
		{
			sort(lines.begin(), lines.end(), comparator);
		}
		
		ofstream out(outfile.c_str());
		
		for (int i=0, len=lines.size(); i<len; ++i)
		{
			out << lines[i] << endl;
		}
		out.close();
	}
	
	struct Item
	{
		ifstream* file;
		string name;
		string line;
		bool is_open;
		bool (*comp) (const string&,const string&);
		
		Item()
		{
			line = "";
			name = "";
			file = NULL;
		}
		
		void open()
		{
			if (name == "")
			{
				cerr << "No filename specified" << endl;
				exit(1);
			}
			file = new ifstream(name.c_str());
		}
		void close()
		{
			line = "";
			file->close();
		}
		bool next()
		{
			return getline(*file, line);
		}
	};

	bool compare_items(const Item& a, const Item& b)
	{
		if (!b.is_open) return true;
		if (a.comp == NULL) return a.line < b.line;
		return a.comp(a.line, b.line);
	}
	
	//Sort a file in chunks then recombine FIXME: get the custom comparitor to pass through to the ITEM
	static void disk_sort(string infile, string outfile, bool (*comparator) (const string&,const string&), int chunk)
	{
		//Split into multiple files
		vector<string> names = chunk_split(infile, outfile, chunk);
		int size = names.size();
		
		vector<Item> files;
		files.resize(size);
		
		//Sort each file
		for (int i=0; i<size; ++i)
		{
			ram_sort(names[i], names[i], comparator);
			files[i].name = names[i];
			files[i].comp = comparator;
			files[i].open();
			files[i].next();
		}
				
		//Merge files back together
		ofstream out(outfile.c_str());
		
		while (!files.empty())
		{
			sort(files.begin(), files.end(), compare_items);

			out << files[0].line << endl;
			
			if (!(files[0].next()))
			{
				files[0].close();
				remove(files[0].name.c_str());
				files.erase(files.begin());
			}
		}
		out.close();
	}
	
	//Sorts the lines in a file by their natural order
	static void file_sort(string infile, string outfile)
	{
		ram_sort(infile, outfile, NULL);
	}

	//Count the number of lines in a file
	static int count_lines(string filename)
	{
		ifstream in(filename.c_str());

		int count = 0;
		string line = "";
		
		while (getline(in,line))
		{
			count++;
		}
		in.close();
		return count;
	}
		
	//Copy a file
	static void copy(string src, string dest)
	{
		ifstream in(src.c_str());
		ofstream out(dest.c_str());

		string line = "";
		
		while (getline(in,line))
		{
			out << line << endl;
		}
		out.close();
		in.close();
	}
	
	//Concatenate a file onto an existing file
	static void concat(string basefile, string file)
	{
		ifstream in(file.c_str());
		ofstream out(basefile.c_str(), ios_base::app);
		
		filebuf * inbuf = in.rdbuf();
		filebuf * outbuf = out.rdbuf();

		char c = inbuf->sgetc();
		
		while (c != EOF)
		{
			outbuf->sputc(c);
			c = inbuf->snextc();
		}
		out.close();
		in.close();
	}
	
	//Concatenate a collection of files into a single file
	static void concat(string basefile, vector<string> files)
	{
		for (int i=0; i<files.size(); i++)
		{
			concat(basefile, files[i]);
		}
	}
	
	//Split a file of reads into a number of files, interleaving the lines
	static void interleave_split(string infile, vector<string> outfiles)
	{
		int numfiles = outfiles.size();
		
		vector<ofstream*> streams;
		streams.resize(numfiles);
		
		for (int i=0; i<numfiles; i++)
		{
			streams[i] = new ofstream();
			streams[i]->open(outfiles[i].c_str());
		}
		
		string line;
		int filenum = 0;
		
		ifstream in(infile.c_str());
				
		while (getline(in,line))
		{
			filenum %= numfiles;
			*(streams[filenum]) << line << endl;
			++filenum;
		}
		in.close();
	
		for (int i=0; i<numfiles; i++)
		{
			streams[i]->close();
			delete streams[i];
		}
		streams.clear();
	}
	
	//Join a collection of files into a single file, interleaving the lines
	static void interleave_join(vector<string> infiles, string outfile)
	{
		int numfiles = infiles.size();
		
		vector<ifstream*> streams;
		streams.resize(numfiles);
		
		for (int i=0; i<numfiles; i++)
		{
			streams[i] = new ifstream();
			streams[i]->open(infiles[i].c_str());
		}
		
		ofstream out(outfile.c_str());
		string line;
	
		while (true)
		{
			int active = 0;
			
			for (int i=0; i<numfiles; i++)
			{
				if (getline(*(streams[i]), line))
				{
					out << line << endl;
					active++;
				}
			}
			if (active == 0) break;
		}
		out.close();
	
		for (int i=0; i<numfiles; i++)
		{
			streams[i]->close();
			delete streams[i];
		}
		streams.clear();
	}
}

#endif
