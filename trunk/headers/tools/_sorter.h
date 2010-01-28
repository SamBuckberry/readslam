#ifndef _READSLAM_SORTER
#define _READSLAM_SORTER

#include "../common/_common.h"
#include "../parsing/_slam.h"

namespace ReadSlam
{
	//Static code for sorting structures or files of .slam reads
	struct Sorter
	{
		//Comparator for sorting on sequence
		static bool compare_sequence(const BasicRead a, const BasicRead b)
		{
			return a.sequence < b.sequence;
		}
		
		//Comparator for sorting on mapped location
		static bool compare_location(const BasicRead a, const BasicRead b)
		{
			if (a.assembly != b.assembly)
			{
				return a.assembly < b.assembly;
			}
			return a.position < b.position;
		}
		
		//Comparator for sorting on native starts (for clone collapse)
		static bool compare_clone(const BasicRead a, const BasicRead b)
		{
			if (a.assembly != b.assembly)
			{
				return a.assembly < b.assembly;
			}
			if (a.strand == "+")
			{
				if (b.strand != "+") return true;
				return a.position < b.position;
			}
			else
			{
				if (b.strand == "+") return false;
				return a.position + a.sequence.size() < b.position + b.sequence.size();
			}
		}
		
		struct FileItem
		{
			ifstream in;
			string name;
			BasicRead read;
			bool ok;
			
			FileItem(string infile)
			{
				name = infile;
				in.open(infile.c_str());
				ok = read.load(in);
			}
			~FileItem()
			{
				in.close();
				name.clear();
			}
			void kill()
			{
				remove(name.c_str());
			}
			void open()
			{
				in.open(name.c_str());
			}
			void close()
			{
				in.close();
			}
			bool next()
			{
				ok = read.load(in);
				return ok;
			}
			void save(ofstream& out)
			{
				if (!ok) return;
				read.save(out);
			}
		};		
		
		static void sort_file(string infile, string outfile, int chunksize, bool (*comparator)(const BasicRead, const BasicRead))
		{
			ifstream in (infile.c_str());
			
			list<FileItem*> files;
			list<FileItem*>::iterator itr;
			files.clear();

			vector<BasicRead> reads;
			reads.resize(chunksize);
			
			//Sort all the reads in chunks and save to separate files
			while (in.good())
			{
				cout << "Batch " << files.size() << ": loading..." << flush;
				
				for (int i=0; i<chunksize; ++i)
				{
					if (!reads[i].load(in))
					{
						reads.resize(i);
						break;
					}
				}
				cout << "sorting..." << flush;
				sort(reads.begin(), reads.end(), comparator);
				cout << "saving..." << flush;
				
				//Save all the reads to a file
				string filename = Strings::add_int(outfile + ".", files.size());
				
				ofstream out (filename.c_str());
				
				for (int i=0, last=reads.size(); i<last; ++i)
				{
					reads[i].save(out);
				}
				out.close();
				
				//Add to the collection of sorted files
				FileItem* f = new FileItem(filename);
				
				bool added = false;
				
				for (itr=files.begin(); itr!=files.end(); ++itr)
				{
					if (comparator(f->read, (*itr)->read))
					{
						added = true;
						files.insert(itr,f);
						break;
					}
				}
				if (!added)
				{
					files.push_back(f);
				}
				cout << "done." << endl;
			}
			in.close();
			
			//Merge the sorted files back together
			ofstream out(outfile.c_str());
			
			while (!files.empty())
			{
				FileItem* f = files.front();
				files.pop_front();
				
				f->save(out);
				
				if (f->next())
				{
					bool added = false;

					for (itr=files.begin(); itr!=files.end(); ++itr)
					{
						if (comparator(f->read, (*itr)->read))
						{
							added = true;
							files.insert(itr,f);
							break;
						}
					}
					if (!added)
					{
						files.push_back(f);
					}
				}
				else
				{
					f->close();
					f->kill();
					delete f;
				}
			}
			out.close();
		}
		
		//Single point entry
		static void sort_reads(string type, int chunksize, string infile, string outfile)
		{
			if (type == "sequence")
			{
				sort_file(infile, outfile, chunksize, compare_sequence);
			}
			else if (type == "location")
			{
				sort_file(infile, outfile, chunksize, compare_location);
			}
			else if (type == "clonal")
			{
				sort_file(infile, outfile, chunksize, compare_clone);
			}
			else
			{
				cerr << "Unrecognized type specified in Sorter::sort_reads: " << type << endl;
				exit(1);
			}
		}
	};
}
#endif