#ifndef _READSLAM_GENOME
#define _READSLAM_GENOME

#include "../common/_sysinfo.h"
#include "_assembly.h"
#include "../parsing/_fastq.h"

/**
 * Represents a reference genome
 * Breaks the reference genome into files, one file per assembly
 * Each file contains one line: the W strand sequence for the sequence
 */
namespace ReadSlam
{
	struct Genome
	{
		vector<Assembly> assemblies;
		Sysinfo system;
		int num_assemblies;
		long length;
		bool bisulfite;

		bool index_built;
		bool index_usemap;
		int  index_seed;
		
		//mapping counters
		long mapped_total;
		long mapped_unique;
		long mapped_multi;
		long mapped_failed;

 		 Genome() { clear(); }
		~Genome() { clear(); }
		
		void clear()
		{
			assemblies.clear();
			num_assemblies = 0;
			length = 0;
			bisulfite = false;

			index_built = false;
			index_usemap = false;
			index_seed = 0;

			mapped_total = 0;
			mapped_unique = 0;
			mapped_multi = 0;
			mapped_failed = 0;
		}
		
		//Load genome from a FastA file
		void load(string infile)
		{
			cout << endl;
			cout << "Loading genome from file: " << infile << endl;
			
			ifstream in (infile.c_str(), fstream::in);

			if (!in)
			{
				cerr << "Error: unable to open reference file " << infile << endl;
				exit(1);
			}
			ostringstream refseq;
			refseq.clear();
			
			vector<string> names;
			vector<long> offsets;

			long offset = 0;
			string line = "";

			while (in.good())
			{
				getline(in, line);

				if (line[0] == '>')
				{
					Strings::replace(line, "chr", "");
					Strings::replace(line, "_u", "");
					Strings::replace(line, " ", "");
					Strings::replace(line, ">", "");
					names.push_back(line);
					offsets.push_back(offset);
					cout << "  - extracting sequence: " << line << " at offset " << offset << endl;
				}
				else
				{
					refseq << line;
					offset += line.size();
				}
			}
			in.close();
			this->num_assemblies = names.size();
			assemblies.resize(num_assemblies);
			
			//Create the assemblies
			{
				cout << endl << "Creating assemblies:" << endl;

				string mega = refseq.str();
				string assembly_sequence = "";
			
				for (int i=0, len=num_assemblies; i<len; ++i)
				{
					cout << "  - creating assembly: " << names[i] << endl;
					
					if (i == len - 1)
					{
						assembly_sequence = mega.substr(offsets[i], offset - offsets[i]);
					}
					else
					{
						assembly_sequence = mega.substr(offsets[i], offsets[i+1] - offsets[i]);
					}
					assemblies[i].init(names[i], assembly_sequence);
				}
			}
			this->length = offset;
			cout << endl;
			cout << "Total genome size: " << this->length << endl;
		}
		
		//Index the genome
		void build_index(int seed, bool bisulfite, bool usemap)
		{
			int max = 0;
			
			if (usemap)
			{
				max = build_index_map(seed, bisulfite);
			}
			else
			{
				max = build_index_raw(seed, bisulfite);
			}
			this->index_seed = max;
			this->index_built = true;
			this->index_usemap = usemap;
			this->bisulfite = bisulfite;
		}
		
		//Build the raw index
		int build_index_raw(int seed, bool bisulfite)
		{
			//Determine the maximum seed size
			int max_seed = 0;
			
			for (int i=0; i<16; ++i)
			{
				long max = (long)pow(4, i);
				long size_idx_idx = (num_assemblies * 3 * max * sizeof(int)) / 1000000;
				long size_idx_seq = (2 * length * sizeof(int)) / 1000000;
				long size_seq = (length * sizeof(char)) / 1000000;
							
				//2 strands * the combined size
				long memreq = 2 * (size_idx_seq + size_idx_idx + size_seq);
				
				if (memreq >= system.ram) 
				{
					break;
				}
				max_seed = i;
			}
			if (seed > max_seed)
			{
				seed = max_seed;
			}
			cout << "System has " << system.ram << "MB RAM" << endl;
			cout << "Maximum seed: " << max_seed << endl;
			cout << "Using seed: " << seed << endl;

			cout << endl << "Indexing:" << endl;
			
			for (int i=0; i<num_assemblies; ++i)
			{
				cout << "  - indexing assembly: " << assemblies[i].name << endl;
				assemblies[i].build_index(seed, bisulfite, false);
			}
			return seed;
		}
		
		//Index the genome using a map
		int build_index_map(int seed, bool bisulfite)
		{
			//Determine the maximum seed size
			int max_seed = 0;
			
			for (int i=0; i<16; ++i)
			{
				long size_idx = (length * sizeof(int)) / 1000000;
				long size_seq = (length * sizeof(char)) / 1000000;
							
				//2 strands * the combined size
				long memreq = 2 * (size_idx + size_seq);
				
				if (memreq >= system.ram) 
				{
					break;
				}
				max_seed = i;
			}
			cout << "System has " << system.ram << "MB RAM" << endl;
			cout << "Maximum seed: " << max_seed << endl;
			cout << "Using seed: " << seed << endl;

			cout << endl << "Indexing:" << endl;
			
			for (int i=0; i<num_assemblies; ++i)
			{
				cout << "  - indexing assembly: " << assemblies[i].name << endl;
				assemblies[i].build_index(seed, bisulfite, true);
			}
			return seed;
		}
		
		//Map a single read to the genome
		void map_read(Read& read)
		{
			if (!index_built)
			{
				cerr << "The index must be built before mapping can be done" << endl;
				exit(1);
			}
			read.build_indices(index_seed, bisulfite);
			
			for (int i=0; i<num_assemblies; ++i)
			{
				if (this->index_usemap)
				{
					read.search_map(assemblies[i].forward);
					read.search_map(assemblies[i].reverse);
				}
				else
				{
					read.search(assemblies[i].forward);
					read.search(assemblies[i].reverse);
				}
			}
			
			if (++mapped_total % 1000 == 0)
			{
				cout << "  - " << mapped_total << "\r" << flush;
			}
			switch (read.locations)
			{
				case 0 : ++mapped_failed; break;
				case 1 : ++mapped_unique; break;
				default: ++mapped_multi;
			}
		}
		
		//Map a file of reads to the genome. Infile and outfile are ReadSlam format
		void map_reads(string infile, string outfile, bool reset_counters)
		{
			if (!index_built)
			{
				cerr << "The index must be built before mapping can be done" << endl;
				exit(1);
			}
			if (reset_counters)
			{
				cout << "Mapping file:" << infile << endl;
				mapped_total = 0;
				mapped_unique = 0;
				mapped_multi = 0;
				mapped_failed = 0;
			}
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());
			
			Read read;
			
			while (read.load(in))
			{
				map_read(read);
				read.save(out);
			}
			out.close();
			in.close();
			
			if (reset_counters)
			{
				cout << "Total: "  << mapped_total << endl;
				cout << "Failed: " << mapped_failed << endl;
				cout << "Unique: " << mapped_unique << endl;
				cout << "Multi: "  << mapped_multi << endl;
			}
		}
		
		//Multi threaded mapping
		struct ThreadDataMap
		{
			ReadSlam::Genome* self;
			string filename;
			
			ThreadDataMap(ReadSlam::Genome* t, string f)
			{
				self = t;
				filename = f;
			}
		};		
		static void* thread_exec_map(void* param)
		{
			ThreadDataMap* data = static_cast<ThreadDataMap*>(param);
			ReadSlam::Genome* self = data->self;

			string infile = data->filename;
			string outfile = infile + ".mapping";
			
			self->map_reads(infile, outfile, false);

			remove(infile.c_str());
			rename(outfile.c_str(), infile.c_str());
			
			delete data;
		}
		//Map a file of reads using multiple threads
		void map_threaded(string infile, string outfile)
		{
			if (!index_built)
			{
				cerr << "The index must be built before mapping can be done" << endl;
				exit(1);
			}
			int numthreads = system.cpus - 1;
			
			cout << "System has " << system.cpus << " CPUs" << endl;
			cout << "Maximum threads: " << numthreads << endl;

			vector<string> readfiles;
			vector<pthread_t> threads;

			readfiles.resize(numthreads);
			threads.resize(numthreads);
			
			//Reset the counters
			mapped_total = 0;
			mapped_unique = 0;
			mapped_multi = 0;
			mapped_failed = 0;

			//Split input file into multiple files (one per thread)
			{
				for (int i=0; i<numthreads; i++)
				{
					readfiles[i] = Strings::add_int(outfile + ".thread.", i);
				}
				cout << "Splitting input for " << numthreads << " threads..." << flush;
				Files::interleave_split(infile, readfiles);
				cout << "done" << endl;
			}

			//Launch threads to map reads
			{
				for (int n=0; n<numthreads; ++n)
				{
					pthread_create(&(threads[n]), NULL, thread_exec_map, new ThreadDataMap(this, readfiles[n]));
				}
				for (int n=0; n<numthreads; ++n)
				{
					pthread_join(threads[n], NULL);
				}
			}

			//Merge results back into a final file
			{
				cout << "Merging results into single file..." << flush;
				Files::interleave_join(readfiles, outfile);
				cout << "done" << endl;
			}

			//Clean up
			{
				cout << "Cleaning up..." << flush;
				for (int i=0; i<numthreads; i++)
				{
					remove(readfiles[i].c_str());
				}
				readfiles.clear();
				cout << "done" << endl;
			}
			
			//Report outcome
			cout << "Total: "  << mapped_total << endl;
			cout << "Failed: " << mapped_failed << endl;
			cout << "Unique: " << mapped_unique << endl;
			cout << "Multi: "  << mapped_multi << endl;
		}		
		
		//Multi-threaded indexing
		struct ThreadDataIndex
		{
			Assembly* assembly;
			bool usemap;
			bool bisulfite;
			int seed;
			
			ThreadDataIndex(Assembly* assembly, bool map, int seed, bool bisulfite)
			{
				this->assembly = assembly;
				this->usemap = map;
				this->seed = seed;
				this->bisulfite = bisulfite;
			}
		};		
		static void* thread_exec_index(void* param)
		{
			ThreadDataIndex* data = static_cast<ThreadDataIndex*>(param);

			data->assembly->build_index(data->seed, data->bisulfite, data->usemap);

			delete data;
		}
		void index_threaded(int seed, bool bisulfite, bool usemap)
		{
			if (!index_built)
			{
				cerr << "The index must be built before mapping can be done" << endl;
				exit(1);
			}
			vector<pthread_t> threads;
			threads.resize(num_assemblies);
			
			//Launch threads to map reads
			{
				for (int i=0; i<num_assemblies; ++i)
				{
					Assembly* a = &(assemblies[i]);
					
					pthread_create(&(threads[i]), NULL, thread_exec_index, new ThreadDataIndex(a,usemap,seed,bisulfite));
				}
				for (int i=0; i<num_assemblies; ++i)
				{
					pthread_join(threads[i], NULL);
				}
			}
		}
	};
}
#endif
