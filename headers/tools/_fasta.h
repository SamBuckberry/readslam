#ifndef _READSLAM_FASTA
#define _READSLAM_FASTA

#include "../common/_common.h"
#include "../core/_dna.h"
#include <map>

/**
 * Various routines for dealing with FastA files
 */
namespace ReadSlam
{
	namespace FastA
	{
		struct Assembly
		{
			string name;
			string sequence;
			int length;
			
			 Assembly() { clear(); }
			~Assembly() { clear(); }
			
			void clear()
			{
				name.clear();
				sequence.clear();
				length = 0;
			}
			void save_multi(ofstream& out)
			{
				char end = '\n';
				
				out << ">" << name << end;
				
				for (int i=0; i<length; i+=80)
				{
					for (int j=i; j<i+80 && j<length; ++j)
					{
						out << sequence[j];
					}
					out << end;
				}
			}
			void save_single(ofstream& out)
			{
				char end = '\n';
				
				out << ">" << name << end;
				out << sequence << end;
			}
			void reverse()
			{
				for (int i=0, mid=length/2; i<mid; ++i)
				{
					char temp = sequence[i];
					sequence[i] = sequence[length-1-i];
					sequence[length-1-i] = temp;
				}
			}
			void complement()
			{
				for (int i=0; i<length; ++i)
				{
					switch(sequence[i])
					{
						case 'A': sequence[i] = 'T'; break;
						case 'T': sequence[i] = 'A'; break;
						case 'C': sequence[i] = 'G'; break;
						case 'G': sequence[i] = 'C'; break;
						default : sequence[i] = 'N'; break;
					}
				}
			}
			void reverse_complement()
			{
				reverse();
				complement();
			}
			void bisulfite()
			{
				for (int i=0; i<length; ++i)
				{
					if (sequence[i] == 'C')
					{
						sequence[i] = 'T';
					}
				}
			}
			bool load(ifstream& in)
			{
				clear();
				
				if (in.peek() != '>') return false;

				getline(in,name);
				name = name.substr(1);
				
				string line;
				ostringstream stream;

				while (getline(in,line))
				{
					stream << line;

					if (in.peek() == '>')
					{
						break;
					}
				}
				sequence = stream.str();
				length = sequence.size();
				return true;
			}
		};

		struct Genome
		{
			vector<Assembly> assemblies;
			
			void clear()
			{
				for (int i=0; i<assemblies.size(); i++)
				{
					assemblies[i].clear();
				}
			}
			
			//Load a FastA file into a memory data object
			void load(string infile)
			{
				assemblies.clear();
			
				Assembly a;

				ifstream in (infile.c_str());

				while (a.load(in))
				{
					assemblies.push_back(a);
				}
				in.close();
			}
		
			//Save a memory resident FastA file to disk
			void save(string outfile, bool multi)
			{
				ofstream out (outfile.c_str());
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					if (multi)
					{
						assemblies[i].save_multi(out);
					}
					else
					{
						assemblies[i].save_single(out);
					}
				}
			}
		
			//Convert a FastA file to single line format
			void single(string infile, string outfile)
			{
				load(infile);
				save(outfile, false);
			}
		
			//Convert a FastA file to multi line format (80 chars)
			void multi(string infile, string outfile)
			{
				load(infile);
				save(outfile, true);
			}			
						
			//Reverse all the sequences in a FastA file
			void reverse(string infile, string outfile)
			{
				load(infile);
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					assemblies[i].reverse();
				}
				save(outfile,true);
			}
		
			//Complement all the sequences in a FastA file
			void complement(string infile, string outfile)
			{
				load(infile);
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					assemblies[i].complement();
				}
				save(outfile,true);
			}
		
			//Reverse complement all the sequences in a FastA file
			void reverse_complement(string infile, string outfile)
			{
				load(infile);
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					assemblies[i].reverse_complement();
				}
				save(outfile,true);
			}		
		
			//Convert all the Cs to Ts in a FastA file (to emulate sodium bisulfite treatment)
			void bisulfite(string infile, string outfile)
			{
				load(infile);
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					assemblies[i].bisulfite();
				}
				save(outfile,true);
			}
		
			//Explode a FastA file into many single sequence FastA files, one file per assembly
			vector<string> explode(string infile, string outbase)
			{
				load(infile);

				vector<string> names;
				ofstream out;
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					string name = outbase + "." + assemblies[i].name;
					names.push_back(name.c_str());
					out.open(name.c_str());
					assemblies[i].save_multi(out);
					out.close();
				}
				return names;
			}
		
			//Join a collection of FastA files together into one file
			void implode(vector<string> infiles, string outfile)
			{
				ofstream out (outfile.c_str());
			
				for (int i=0; i<infiles.size(); ++i)
				{
					ifstream in (infiles[i].c_str());
					string line = "";
				
					while (getline(in, line))
					{
						out << line << '\n';
					}
					in.close();
				}
				out.close();
			}
		
			//Replace an existing header with a new one in a FastA file (remember the '>')
			void header_replace(string stale, string fresh, string infile, string outfile)
			{
				load(infile);

				for (int i=0; i<assemblies.size(); ++i)
				{
					if (assemblies[i].name == stale)
					{
						assemblies[i].name = fresh;
					}
				}
				save(outfile,true);
			}
		
			//Add a header to a sequence file
			void header_add(string header, string infile, string outfile)
			{
				load(infile);
				assemblies[0].name = header;
				save(outfile,true);
			}
		
			//Strip the first header line from a FastA file
			void header_remove(string infile, string outfile)
			{
				load(infile);
				assemblies[0].name = "";
				save(outfile,true);
			}
		
			//Grab a particular assembly
			int get(string assembly)
			{
				for (int i=0; i<assemblies.size(); ++i)
				{
					if (assemblies[i].name == assembly)
					{
						return i;
					}
				}
				return -1;
			}
		
			//Count the number of bases per assembly
			map<string,int> count()
			{
				map<string,int> counts;
				counts.clear();
			
				for (int i=0; i<assemblies.size(); ++i)
				{
					counts[assemblies[i].name] = assemblies[i].length;
				}
				return counts;
			}
			
			//Print information to std::out
			void diagnose(string infile)
			{
				load(infile);
				map<string,int> counts = count();
				map<string,int>::iterator it;
				
				for (it = counts.begin(); it != counts.end(); ++it)
				{
					cout << it->first << ": " << it->second << endl;
				}
			}
		};
	}
}
#endif
