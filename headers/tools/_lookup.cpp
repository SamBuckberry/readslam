#include "_fasta.h"

namespace ReadSlam
{
	struct Lookup
	{
		FastA::Genome genome;
		
		void build(string infile)
		{
			genome.load(infile);
		}
		void lookup(string assembly, string strand, int start, string target)
		{
			for (int i=0; i<genome.assemblies.size(); ++i)
			{
				if (genome.assemblies[i].name == assembly)
				{
					if (start < 0 || start + target.size() >= genome.assemblies[i].sequence.size())
					{
						cout << "Illegal coordinates" << endl;
						return;
					}
					string s = genome.assemblies[i].sequence.substr(start,target.size());
					cout << "\n" << s << "\n" << target << "\n" << DNA::reverse_complement(s) << endl;
					return;
				}
			}
			cout << "not found" << endl;
		}
	};
}
int main (int argc, const char* argv[])
{
	if (argc != 6)
	{
		cout << "Usage: ./lookup genome.fa assembly strand start sequence" << endl;
		cout << "Example: ./lookup human.fasta chrX + 100000 AAATATGCGTATCGTATTCAGACGAGCTGATCTGT" << endl;
		return 1;
	}
	ReadSlam::Lookup l;
	
	l.build(argv[1]);
	l.lookup(argv[2], argv[3], atoi(argv[4]), argv[5]);
	
	return 0;
}