#ifndef _READSLAM_READ
#define _READSLAM_READ

#include <string>
#include <fstream>
#include <cstring>
#include <list>
#include <limits>
#include "../common/_common.h"
#include "_dna.h"
#include "_sequence.h"

namespace ReadSlam
{			
	struct ReadIndex
	{
		int val;
		int pos;
		int min;
		int gs;
	};
	
	static bool compare_index(ReadIndex a, ReadIndex b)
	{
		if (a.min == b.min) return b.gs < a.gs;
		return b.min < a.min;
	}
	
	struct Read
	{
		string name;
		string sequence;
		string qualities;
		string assembly;
		string strand;
		bool forward;
		bool bisulfite;
		int length;
		int score;
		int mismatches;
		int copies;
		int locations;
		int position;
		int seed;
			
		//Indices, sorted by the highest minimum
		vector<ReadIndex> indices;
		
		 Read() { clear(); }
		~Read() { clear(); }
		
		void clear()
		{
			name = ".";
			sequence = ".";
			qualities = ".";
			assembly = ".";
			
			forward   = true;
			bisulfite = false;
			
			mismatches = numeric_limits<int>::max();
			locations  = 0;
			position   = 0;
			copies = 0;
			length = 0;
			score  = numeric_limits<int>::max();
			seed = 0;
			
			indices.clear();
		}
		
		bool load(ifstream& in)
		{
			if (!(in
				>> locations
				>> mismatches
				>> score
				
				>> assembly
				>> strand
				>> position
				
				>> name
				>> copies
				>> sequence
				>> qualities
			)) return false;
			
			forward = strand == "+";
			length = sequence.size();
			return true;
		}
		
		//Save the read to file
		void save(ofstream& out)
		{
			char tab = '\t';
			
			out << locations
				<< tab << mismatches
				<< tab << score

				<< tab << assembly
				<< tab << (forward ? "+" : "-")
				<< tab << position

				<< tab << name
				<< tab << copies
				<< tab << sequence
				<< tab << qualities
			<< '\n';
		}
		
		bool from_string(string& line)
		{
			vector<string> fields = Strings::split(line, '\t');

			if (fields.size() != 10)
			{
				cerr << "Bad field count. Should be 10, found: " << fields.size() << endl;
				return false;
			}
			
			//Extract raw values
			locations  = atoi(fields[0].c_str());
			mismatches = atoi(fields[1].c_str());
			score      = atoi(fields[2].c_str());

			assembly   = fields[3];
			strand     = fields[4];
			position   = atoi(fields[5].c_str());

			name       = fields[6];
			copies     = atoi(fields[7].c_str());
			sequence   = fields[8];
			qualities  = fields[9];

			forward = strand == "+";
			length = sequence.size();
			return true;
		}
		
		string to_string()
		{
			ostringstream s;
			char tab = '\t';
			
			s << locations
				<< tab << mismatches
				<< tab << score

				<< tab << assembly
				<< tab << (forward ? "+" : "-")
				<< tab << position

				<< tab << name
				<< tab << copies
				<< tab << sequence
				<< tab << qualities
			<< '\n';
			
			return s.str();
		}
		
		//Dump to screen for debugging
		void dump()
		{
			cout << name << endl;
			cout << " >> " << sequence << endl;
			cout << " >> " << qualities << endl;
			cout << "Length: " << length << endl;
			
			for (int i=0; i<indices.size(); i++)
			{
				cout << indices[i].pos << "\t" << indices[i].min << "\t" << indices[i].gs << "\t" << indices[i].val << endl;
			}
		}
		
		//Returns true if the other read is clonal with this one
		bool clonal(Read& read)
		{
			if (read.assembly != assembly) return false;
			if (read.forward != forward) return false;
			
			if (forward)
			{
				return read.position == position;
			}
			else
			{
				return read.position + read.length == position + length;
			}
		}
		
		//Build the indices for this read
		void build_indices(int seed, bool bisulfite)
		{
			this->bisulfite = bisulfite;
			this->seed = seed;

			vector<int> raw_indices;
			DNA::seq2indices(sequence, raw_indices, seed, bisulfite);
			
			indices.resize(length);

			for (int i=0; i<length; ++i)
			{
				indices[i].val = raw_indices[i];
				indices[i].pos = i;
				indices[i].min = qualities[i];
				indices[i].gs = 0;
				
				if (indices[i].val == -1 || i+seed > length)
				{
					indices[i].min = 0;
					continue;
				}
				
				for (int j=0; j<seed; ++j)
				{
					if (sequence[i+j] == 'G')
					{
						indices[i].gs++;
					}
					if (qualities[i+j] < indices[i].min)
					{
						indices[i].min = qualities[i+j];
					}
				}
			}
			
			//Sort the indices
			std::sort(indices.begin(), indices.end(), compare_index);
		}
		
		//Search using the vector approach
		void search(Sequence& s)
		{
			for (int i=0; i<length; ++i)
			{
				int idx = indices[i].val;
				if (idx == -1) continue;

				int count = s.index_counts[idx];
				if (count == 0) continue;

				int offset = s.index_offsets[idx];
				int pos_read = indices[i].pos;

				for (int p=0; p<count; ++p)
				{
					int pos_genome = s.index_sorted[offset + p] - pos_read;
					
					if (pos_genome < 0 || pos_genome + length > s.length)
					{
						continue;
					}
					align(s, pos_genome);
				}
				break;
			}
		}
		
		//Search using the map approach
		void search_map(Sequence& s)
		{
			list<int>* locations;
			list<int>::iterator it;
			
			for (int i=0; i<length; ++i)
			{
				locations = s.lookup(indices[i].val);
				
				if (locations == NULL) continue;
				
				int pos_read = indices[i].pos;
		
				for (it = locations->begin(); it != locations->end(); it++ )
				{
					long pos_genome = *it - pos_read;
					
					if (pos_genome < 0 || pos_genome + length > s.length)
					{
						continue;
					}
					align(s, pos_genome);
				}
				break;
			}
		}
		
		//Specific alignment of read to reference sequence
		void align(Sequence& s, long pos)
		{
			int tally = 0;
			int fails = 0;
		
			for (int i=length-1; i>=0; --i)
			{
				char charRead = sequence[i];
				char charRef  = s.sequence[pos+i];
			
				if (charRead == charRef) continue;
				if (bisulfite && charRef == 'C' && charRead == 'T') continue;
				
				tally += qualities[i];
				
				if (tally > score) return;
				fails++;
			}
			
			//Deal with a multi
			if (tally == score)
			{
				if (position != s.forward ? pos : s.length - pos - length)
				{
					locations++;
				}
				return;
			}
			
			//Store the new, best hit
			locations  = 1;
			score      = tally;
			mismatches = fails;
			assembly   = s.name;
			forward    = s.forward;
			position   = s.forward ? pos : s.length - pos - length;
		}		
	};
};
#endif