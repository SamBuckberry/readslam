#ifndef _READSLAM_METHSTAT
#define _READSLAM_METHSTAT

#include "../common/_common.h"
#include "../tools/_fasta.h"
#include "../tools/_fasta.h"
#include "../parsing/_slam.h"

//Takes a reference genome and a file of mc sites and makes all C lower case in the ref genome where meth (lower G for - strand)
namespace ReadSlam
{
	struct MethSite
	{
		string assembly;
		int position;
		char strand;
		string context;
		short int c;
		short int h;

		bool load(ifstream& in)
		{
			return in 
				>> assembly
				>> position
				>> strand
				>> context
				>> c
				>> h
			;
		}
		bool save(ofstream& out)
		{
			char tab = '\t';
			char end = '\n';
			
			out << assembly
				<< tab << position
				<< tab << strand
				<< tab << context
				<< tab << c
				<< tab << h
			<< end;
		}
	};
	struct MethStat
	{
		map<string,string> genome;
		map<string,string>::iterator itr;
		string dna;

		void build(string mc_file, string infile, string outfile)
		{
			load_genome(infile);
			process(mc_file);
			save(outfile);
		}
		
		//Load the genome
		void load_genome(string infile)
		{
			cout << "Loading genome" << endl;
			FastA::Genome f;
			f.load(infile);
			
			for (int i=0; i<f.assemblies.size(); i++)
			{
				string name = f.assemblies[i].name;
				Strings::replace(name, "chr", "");
				Strings::replace(name, "_u", "");
				Strings::replace(name, " ", "");
				Strings::replace(name, ">", "");
				genome[name] = f.assemblies[i].sequence;
			}
			cout << "done" << endl;
		}
		
		//Process an mC file and use it to update the genome
		void process(string infile)
		{
			ifstream in (infile.c_str());
			MethSite m;
			
			while (m.load(in))
			{
				if (m.strand == '+')
				{
					if (genome[m.assembly][m.position] == 'C')
					{
						genome[m.assembly][m.position] = 'c';
					}
					else
					{
						cerr << "Reference is not a C at position " << m.position << endl;
						exit(1);
					}
				}
				else
				{
					if (genome[m.assembly][m.position] == 'G')
					{
						genome[m.assembly][m.position] = 'g';
					}
					else
					{
						cerr << "Reference is not a G at position " << m.position << endl;
						exit(1);
					}
				}
			}
		}
		
		//Save modified genome
		void save(string outfile)
		{
			ofstream out (outfile.c_str());
			
			map<string,string>::iterator itr;
			
			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				out << '>' << itr->first << '\n' << itr->second << '\n';
			}
			out.close();
		}
		
		//Build the genome as a single dna string
		void build_dna(string infile)
		{
			ifstream in (infile.c_str());
			
			ostringstream s;
			string line;
			
			while (getline(in,line))
			{
				if (line.size() == 0) continue;
				if (line[0] == '>') continue;
				s << line;
			}
			dna = s.str();
			in.close();
		}
		
		//Count occurrences of single bases relative to meth site
		void range(string infile, int up, int dn, bool mc)
		{
			if (up > dn)
			{
				cerr << "Illegal input, upstream value must be <= downstream value" << endl;
				return;
			}
			build_dna(infile);
			
			map<string,unsigned int> counts;
			map<string,unsigned int>::iterator itr;
			string tag;
			
			for (unsigned int i=0, len=dna.size(); i<len; ++i)
			{
				if (mc)
				{
					if (dna[i] != 'c') continue;
				}
				else
				{
					if (dna[i] != 'C') continue;
				}
				if (i + up < 0) continue;
				if (i + dn < 0) continue;

				if (i + up >= len) break;
				if (i + dn >= len) break;
				
				tag = dna.substr(i+up, 1+dn-up);
				
				itr = counts.find(tag);

				if (itr == counts.end())
				{
					counts[tag] = 1;
				}
				else
				{
					counts[tag]++;
				}
			}
			for (itr=counts.begin(); itr!=counts.end(); ++itr)
			{
				cout << (itr->first) << '\t' << itr->second << endl;
			}
		}
		
		//Count the number of motifs in a reference genome (+ strand only)
		int motif_count(string infile, string motif)
		{
			build_dna(infile);
			
			int count = 0;
			int taglen = motif.size();
			
			for (unsigned int i=0, len=dna.size(); i<len; ++i)
			{
				for (int j=0; j<taglen; ++j)
				{
					if (motif[j] != dna[i]) break;
					if (j == taglen-1) ++count;
				}
			}
			return count;
		}
		
		//Dump all the motifs in the specified range with no collapse
		void motif_dump(string infile, int up, int dn)
		{
			if (up > dn)
			{
				cerr << "Illegal input, upstream value must be <= downstream value" << endl;
				return;
			}
			build_dna(infile);
			
			for (unsigned int i=0, len=dna.size(); i<len; ++i)
			{
				if (dna[i] != 'c') continue;

				if (i + up < 0) continue;
				if (i + dn < 0) continue;

				if (i + up >= len) break;
				if (i + dn >= len) break;
				
				cout << dna.substr(i+up, 1+dn-up) << endl;
			}
		}
		
		//List all the locations where a specific motif exists
		void position(string ref, string motif)
		{
			load_genome(ref);
			
			int taglen = motif.size();
			
			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				for (unsigned int i=0, len=itr->second.size(); i<len; ++i)
				{
					for (int j=0; j<taglen; ++j)
					{
						if (motif[j] != itr->second[i+j]) break;
						if (j == taglen-1) cout << itr->first << '\t' << i << endl;
					}
				}
			}
		}

		//List all motifs relative to the positions in a file
		void relative(string ref, string nums, int up, int dn)
		{
			build_dna(ref);
			unsigned int len = dna.size();
			
			ifstream in(nums.c_str());
			
			map<string,unsigned int> counts;
			map<string,unsigned int>::iterator itr;
			string tag;
			
			unsigned int i;
			
			while (in >> i)
			{
				if (i + up < 0) continue;
				if (i + dn < 0) continue;

				if (i + up >= len) break;
				if (i + dn >= len) break;
				
				if (up > dn) break;
				
				tag = dna.substr(i+up, 1+dn-up);
				
				itr = counts.find(tag);

				if (itr == counts.end())
				{
					counts[tag] = 1;
				}
				else
				{
					counts[tag]++;
				}
			}
			in.close();
			
			for (itr=counts.begin(); itr!=counts.end(); ++itr)
			{
				cout << (itr->first) << '\t' << itr->second << endl;
			}
		}
			
		//Generate a methlation profile for the regions upstream of the provided elements
		void profile_up(string ref, string elements, int up, int dn)
		{
			load_genome(ref);
			
			BasicRead r;
			string* s;
			string tag;
			ifstream in (elements.c_str());
			
			int taglen = 1 + dn - up;
			vector<int> counts;
			counts.resize(taglen,0);
			
			while (r.load(in))
			{
				if (r.strand == "-") continue;
				
				s = &genome[r.assembly];
				
				up = r.position + up;
				if (up < 0) up = 0;
				
				dn = r.position + dn;
				if (dn >= s->size()) dn = s->size() - 1;
				
				taglen = 1 + dn - up;
				tag = s->substr(up, taglen);
				
				for (int i=0; i<taglen; ++i)
				{
					if (tag[i] == 'c')
					{
						counts[i]++;
					}
				}
			}
			in.close();
			
			for (int i=0; i<counts.size(); ++i)
			{
				cout << (i+up) << '\t' << counts[i] << endl;
			}
		}
		
		//Generate a methlation profile for the regions downstream of the provided elements
		void profile_dn(string ref, string elements, int up, int dn)
		{
			load_genome(ref);
			
			BasicRead r;
			string* s;
			string tag;
			ifstream in (elements.c_str());
			
			int taglen = 1 + dn - up;
			vector<int> counts;
			counts.resize(taglen,0);
			
			while (r.load(in))
			{
				if (r.strand == "-") continue;
				
				s = &genome[r.assembly];
				
				up = r.position + r.sequence.size() + up - 1;
				if (up < 0) up = 0;
				
				dn = r.position + r.sequence.size() + dn - 1;
				if (dn >= s->size()) dn = s->size() - 1;

				taglen = 1 + dn - up;
				tag = s->substr(up, taglen);
				
				for (int i=0; i<taglen; ++i)
				{
					if (tag[i] == 'c')
					{
						counts[i]++;
					}
				}
			}
			in.close();
			
			for (int i=0; i<counts.size(); ++i)
			{
				cout << (i+up) << '\t' << counts[i] << endl;
			}
		}
		
		//Generate a relative % internal profile for methlation in the provided elements
		void profile_in(string ref, string elements)
		{
			load_genome(ref);
			
			BasicRead r;
			string* s;
			string tag;
			ifstream in (elements.c_str());
			
			vector<float> counts;
			int sum = 0;
			counts.resize(100,0);
			
			while (r.load(in))
			{
				if (r.strand == "-") continue;
				
				s = &genome[r.assembly];
				
				tag = s->substr(r.position, r.sequence.size());
				sum = 0;
				
				vector<int> temp;
				temp.resize(100,0);
				
				for (int i=0, len=tag.size(); i<len; ++i)
				{
					if (tag[i] == 'c')
					{
						temp[i/len]++;
						sum++;
					}
				}
				for (int i=0; i<100; ++i)
				{
					counts[i] += (float)temp[i] / (float)sum;
				}
			}
			in.close();
			
			for (int i=0; i<counts.size(); ++i)
			{
				cout << i << '\t' << (int)counts[i] << endl;
			}
		}		
	};
}
#endif