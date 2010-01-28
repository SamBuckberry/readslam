#ifndef _READSLAM_MAPPER_SE
#define _READSLAM_MAPPER_SE

#include "_common.h"
#include "_genome.h"

/**
 * Provides multi-threading for read mapping
 */
namespace ReadSlam
{
	struct Mapper
	{
		Genome genome;
		int progress;
		
		//Single thread code
		void *run_thread(void *infile_ptr)
		{
			string infile = *((string *) infile_ptr);
			string outfile = infile + ".mapping";

			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());

			in.open(infile.c_str(), fstream::in);
			out.open(outfile.c_str(), fstream::out);

			//Reads are processed in batches of 1,000,000
			int batch_size = 1000000;
			vector<Read> reads;
			reads.resize(batch_size);
			
			while (in.good() && !in.eof())
			{
				int loaded = 0;
				
				//Load reads
				for (int i=0; i<batch_size; ++i)
				{
					if (!reads[i].load(in)) break;
					loaded++;
				}
				
				//Map reads
				for (int i=0; i<loaded; ++i)
				{
					++progress;
					genome.map(read);
				}
				
				//Save reads
				for (int i=0; i<loaded; ++i)
				{
					reads[i].save(out);
				}
				cout << "Mapped " << progress << " reads                  \r" << flush;
			}
			out.close();
			in.close();

			remove(infile.c_str());
			rename(outfile.c_str(), infile.c_str());
		}	

		//Multithreaded mapping initiation
		void run(string infile, string outfile, int numthreads)
		{
			vector<string> readfiles;
			vector<pthread_t> threads;

			readfiles.resize(numthreads);
			threads.resize(numthreads);

			//Split input for threads
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
				progress = 0;

				for (int n=0; n<numthreads; ++n)
				{
					pthread_create(&(threads[n]), NULL, run_thread, (void*) &(readfiles[n]));
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
		}
		
		void map(Genome& g, string infile, string outfile)
		{
			genome = g;
			
			ifstream in (infile.c_str());
			ofstream out (outfile.c_str());
			
			long total = 0;
			long unique = 0;
			long multi = 0;
			long failed = 0;
			
			FastQ fq;
			Read read;
			
			cout << endl;
			cout << "Mapping:" << endl;
			
			while (fq.load(in))
			{
				fq.to_read(read);
				read.build_indices(seed, bisulfite);
				
				if (++total % 1000 == 0)
				{
					cout << "  - " << total << "\r" << flush;
				}
				for (int i=0; i<num_assemblies; ++i)
				{
					read.search(assemblies[i].forward);
					read.search(assemblies[i].reverse);
				}
				if (read.locations == 0)
				{
					failed++;
					//read.dump();
				}
				else if (read.locations == 1)
				{
					unique++;
					read.save(out);
				}
				else
				{
					multi++;
				}
			}
			out.close();
			in.close();
			
			cout << "  - " << total << endl;
			cout << "Total: " << total << endl;
			cout << "Failed: " << failed << endl;
			cout << "Unique: " << unique << endl;
			cout << "Multi: " << multi << endl;
		}		
	};	
}
#endif