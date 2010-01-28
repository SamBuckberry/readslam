#ifndef _READSLAM_METHSTAT
#define _READSLAM_METHSTAT

#include "../common/_common.h"
#include "../tools/_fasta.h"
#include "../core/_dna.h"
#include "../parsing/_slam.h"
#include <cstdlib>
#include <map>

namespace ReadSlam
{
	//Various routines for doing read and meth analysis
	struct Analysis
	{
		map<string,string> genome;
		map<string,string>::iterator itr;
		
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
		
		//Compress a string of DNA
		string compress(string& seq)
		{
			ostringstream s;
			char c;
			int length = seq.size();

			for (int i=0; i<length-8; i+=8)
			{
				c = 0;
				
				for (int j=0; j<8; ++j)
				{
					switch (seq[i+j])
					{
						case 'A' : c <<= 2; break;
						case 'C' : c <<= 2; c++; break;
						case 'G' : c <<= 1; c++; c <<= 1; break;
						case 'T' : c <<= 1; c++; c <<= 1; c++; break;
						default : c <<= 2; break;
					}
				}
				s << c;
			}
			return s.str();
		}
		
		//Determine genomic repetition at N bases
		void multiplicity(string ref, int n, bool bs)
		{
			load_genome(ref);
			
			string outfile = ref + ".tmp";
			ofstream out(outfile.c_str());
			
			//n /= 8;
			
			map<string,int> counts;
			map<string,int>::iterator citr;
			
			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				cout << " - processing assembly: " << itr->first << endl;
				
				string f = itr->second;
				string r = DNA::reverse_complement(itr->second);
				
				if (bs)
				{
					cout << " - converting to bisulfite" << endl;
					f = DNA::bisulfite(f);
					r = DNA::bisulfite(r);
				}
				
				//Compress
				//cout << " - compressing" << endl;
				//f = compress(f);
				//r = compress(r);

				//forward strand
				cout << " - counting + strand" << endl;
				
				for (int i=0, len=f.size() - n; i<len; ++i)
				{
					if (i % 100000 == 0)
					{
						cout << " " << i << "\r" << flush;
					}
					string tag = f.substr(i,n);
					out << tag << '\n';
					// citr = counts.find(tag);
					// 
					// if (citr == counts.end())
					// {
					// 	counts[tag] = 0;
					// }
					// counts[tag]++;
				}

				//reverse strand
				cout << " - counting - strand" << endl;

				for (int i=0, len=r.size() - n; i<len; ++i)
				{
					if (i % 100000 == 0)
					{
						cout << " " << i << "\r" << flush;
					}
					string tag = r.substr(i,n);
					out << tag << '\n';
					// citr = counts.find(tag);
					// 
					// if (citr == counts.end())
					// {
					// 	counts[tag] = 0;
					// }
					// counts[tag]++;
				}
			}
			//cout << " - retrieving result" << endl;
			
			//Report results
			// unsigned int unique = 0;
			// 
			// for (citr=counts.begin(); citr!=counts.end(); ++citr)
			// {
			// 	cout << citr->first << endl;
			// 	if (citr->second == 1) ++unique;
			// }
			// cout << "Unique: " << unique << endl;
			
			out.close();
			cout << " - sorting" << endl;
			Files::disk_sort(outfile, outfile + ".sorted", NULL, 100000000);
			
			ifstream in((outfile + ".sorted").c_str());
			
			cout << " - counting" << endl;

			string last = "";
			string now = "";
			unsigned long unique = 0;
			unsigned long total = 0;
			
			while (getline(in,now))
			{
				++total;
				if (now != last) ++unique;
				last = now;
			}
			cout << unique << " / " << total << endl;
		}
		
		//Take a file of reads and produce an equivalent file of random reads
		void randomize(string ref, string infile, string outfile)
		{
			load_genome(ref);
			
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());

			string assembly;
			string strand;
			string sequence;
			int start;
			
			while (in >> assembly >> strand >> start >> sequence)
			{
				int len = sequence.size();
				long size = genome[assembly].size();
				long pos = (long)(((double)rand() / (double)RAND_MAX) * (size - len));

				out << assembly << '\t' << '+' << '\t' << pos << '\t' << genome[assembly].substr(pos,len) << '\n';
			}
			out.close();
			in.close();
		}
		
		//Count the number of substrings in another string
		unsigned long motif_count(const string& sequence, const string& motif)
		{
			unsigned long seqlen = sequence.size();
			unsigned long motlen = motif.size();
			unsigned long tally = 0;
		
			for (unsigned long i=0; i<seqlen - motlen; ++i)
			{
				for (unsigned long j=0; j<motlen; ++j)
				{
					if (motif[j] != sequence[i+j])
					{
						if (motif[j] == 'H')
						{
							if (sequence[i+j] == 'G' || sequence[i+j] == 'g')
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
					if (j == motlen - 1) ++tally;
				}
			}
			return tally;
		}
				
		//Given a file of regions determine the count of a given motif within those regions
		long motif_count_in_regions(string ref, string haystacks, string needle)
		{
			load_genome(ref);
			
			ifstream in (haystacks.c_str());
			
			string assembly;
			string strand;
			unsigned long start;
			unsigned long tally = 0;
			string sequence;
			string haystack;
			
			while (in >> assembly >> strand >> start >> sequence)
			{
				//should put some sanity checking in here
				haystack = genome[assembly].substr(start, sequence.size());
				tally += motif_count(haystack, needle);
			}
			in.close();
			
			return tally;
		}
		
		//Count the number of times a motif occurs in the genome
		long motif_count_in_genome(string ref, string needle)
		{
			load_genome(ref);
			
			unsigned long tally = 0;
			
			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				tally += motif_count(itr->second, needle);
			}
			return tally;
		}
		
		//Generate meth analysis for a genome
		void meth_genome(string ref)
		{
			load_genome(ref);

			unsigned long tally_C = 0;
			unsigned long tally_CG = 0;
			unsigned long tally_CHG = 0;
			unsigned long tally_CHH = 0;
			unsigned long tally_c = 0;
			unsigned long tally_cG = 0;
			unsigned long tally_cHG = 0;
			unsigned long tally_cHH = 0;
			
			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				int len = itr->second.size();

				for (int i=0; i<len; ++i)
				{
					if (itr->second[i] == 'g')
					{
						itr->second[i] = 'G';
					}
				}
				tally_C   += motif_count(itr->second, "C");
				tally_CG  += motif_count(itr->second, "CG");
				tally_CHG += motif_count(itr->second, "CHG");
				tally_CHH += motif_count(itr->second, "CHH");
				tally_c   += motif_count(itr->second, "c");
				tally_cG  += motif_count(itr->second, "cG");
				tally_cHG += motif_count(itr->second, "cHG");
				tally_cHH += motif_count(itr->second, "cHH");
			}
			cout << "C:"   << tally_C   << endl;
			cout << "CG:"  << tally_CG  << endl;
			cout << "CHG:" << tally_CHG << endl;
			cout << "CHH:" << tally_CHH << endl;
			cout << "c:"   << tally_c   << endl;
			cout << "cG:"  << tally_cG  << endl;
			cout << "cHG:" << tally_cHG << endl;
			cout << "cHH:" << tally_cHH << endl;
		}
		
		//Generate meth analysis for a file of reads against a genome
		void meth_reads(string ref, string infile)
		{
			load_genome(ref);

			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				int len = itr->second.size();

				for (int i=0; i<len; ++i)
				{
					if (itr->second[i] == 'g')
					{
						itr->second[i] = 'G';
					}
				}
			}

			unsigned long tally_C = 0;
			unsigned long tally_CG = 0;
			unsigned long tally_CHG = 0;
			unsigned long tally_CHH = 0;
			unsigned long tally_c = 0;
			unsigned long tally_cG = 0;
			unsigned long tally_cHG = 0;
			unsigned long tally_cHH = 0;
						
			string assembly;
			string strand;
			unsigned long start;
			unsigned long tally = 0;
			string sequence;
			string haystack;
			
			ifstream in (infile.c_str());

			while (in >> assembly >> strand >> start >> sequence)
			{
				//should put some sanity checking in here
				haystack = genome[assembly].substr(start, sequence.size());
				tally_C   += motif_count(haystack, "C");
				tally_CG  += motif_count(haystack, "CG");
				tally_CHG += motif_count(haystack, "CHG");
				tally_CHH += motif_count(haystack, "CHH");
				tally_c   += motif_count(haystack, "c");
				tally_cG  += motif_count(haystack, "cG");
				tally_cHG += motif_count(haystack, "cHG");
				tally_cHH += motif_count(haystack, "cHH");
			}
			in.close();

			cout << "C:"   << tally_C   << endl;
			cout << "CG:"  << tally_CG  << endl;
			cout << "CHG:" << tally_CHG << endl;
			cout << "CHH:" << tally_CHH << endl;
			cout << "c:"   << tally_c   << endl;
			cout << "cG:"  << tally_cG  << endl;
			cout << "cHG:" << tally_cHG << endl;
			cout << "cHH:" << tally_cHH << endl;
		}
		
		//Get counts for G,C,g,c in a file of regions
		void hm_stat(string ref, string infile, string outfile)
		{
			load_genome(ref);
						
			string assembly;
			int id;
			int start;
			int end;
			int gc;
			int GC;
			string type;
			
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());

			while (in >> id >> assembly >> start >> end >> gc >> GC >> type)
			{
				if (start < 0 || end >= genome[assembly].size())
				{
					continue;
				}
				string haystack = genome[assembly].substr(start, 1 + end - start);

				gc += motif_count(haystack, "gc");
				gc += motif_count(haystack, "cg");

				GC += motif_count(haystack, "GC");
				GC += motif_count(haystack, "CG");

				GC += gc;
				
				out << id 
					<< '\t' << assembly
					<< '\t' << start
					<< '\t' << end
					<< '\t' << gc
					<< '\t' << GC
					<< '\t' << type 
				<< '\n';
			}
			out.close();
			in.close();
		}
		
		//Generate a randomized version of an hm_meth file
		void hm_randomize(string ref, string infile, string outfile)
		{
			load_genome(ref);

			string assembly;
			int id;
			int start;
			int end;
			int gc;
			int GC;
			string type;
			
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());

			while (in >> id >> assembly >> start >> end >> gc >> GC >> type)
			{
				int len = 1 + end - start;
				long size = genome[assembly].size();
				
				if (start < 0 || end >= size)
				{
					continue;
				}
				long pos = (long)(((double)rand() / (double)RAND_MAX) * (size - len));

				out << id 
					<< '\t' << assembly
					<< '\t' << pos
					<< '\t' << pos + len - 1
					<< '\t' << 0
					<< '\t' << 0
					<< '\t' << type 
				<< '\n';
			}
			out.close();
			in.close();
		}
		
		struct element
		{
			string assembly;
			vector<int> meth;
			vector<int> total;
		};
		//Generate data to plot the methylation density across the chromosome
		void hm_plot(string ref)
		{
			load_genome(ref);

			vector<element> elements;
			int limit = 1000;

			for (itr=genome.begin(); itr!=genome.end(); ++itr)
			{
				elements.resize(elements.size()+1);
				elements.back().assembly = itr->first;
				elements.back().meth.resize(limit,0);
				elements.back().total.resize(limit,0);
				 
				int len = itr->second.size();
				int bin = len / limit;
				
				//cout << itr->first << '\t' << bin << '\n';
				
				for (int i=0; i<limit; ++i)
				{
					int start = i * bin;
					
					for (int j=start; j<start+bin && j<len; ++j)
					{
						if (itr->second[j] == 'c')
						{
							elements.back().meth[i]++;
							elements.back().total[i]++;
						}
						else if (itr->second[j] == 'C')
						{
							elements.back().total[i]++;
						}
					}
				}
			}
			//Print the data
			for (int i=0; i<elements.size(); ++i)
			{
				for (int j=0; j<limit; j++)
				{
					cout << elements[i].assembly 
						<< '\t' << j
						<< '\t' << elements[i].meth[j]
						<< '\t' << elements[i].total[j]
					<< '\n';
				}
				cout << flush;
			}
		}
				
		/*
		//Filter a file of reads to include/exclude reads that cover meth sites
		void filter(string ref, string infile, string outfile, bool sense);

		//Convert read sequences into 'methized' read sequences against a methized genome
		void methize(string ref, string infile, string outfile);
		
		//Determine what % of reads cover at least 1 meth site (flag for sense/antisense)
		float coverage_read(string ref, string infile, bool sense);
		
		//Determine what % of genomic meth sites are covered by reads (flat for sense/antisense)
		float coverage_genome(string ref, string infile, bool sense);
		
		//Build base composition numbers for a file of reads relative to 5' end
		BaseCounts composition_5(string ref, string infile, int up, int dn);
		
		//Build base composition numbers for a file of reads relative to 3' end
		BaseCounts composition_3(string ref, string infile, int up, int dn);
		*/
	};
}
#endif