#ifndef _READSLAM_MC
#define _READSLAM_MC

#include "../common/_common.h"
#include "../tools/_fasta.h"
#include <queue>

namespace ReadSlam
{
	namespace MC
	{
		struct Stack
		{
			string assembly;
			int position;
			char fref, fcall;
			int fa, ft, fc, fg, ftotal;

			char rref, rcall;
			int ra, rt, rc, rg, rtotal;
		
			bool load(ifstream& in)
			{
				return (in
					>> assembly
					>> position

					>> fref >> fcall
					>> fa >> ft >> fc >> fg >> ftotal

					>> rref >> rcall
					>> ra >> rt >> rc >> rg >> rtotal
				);
			}
		};
	
		struct Builder
		{
			map<string, string> genome;
			queue<Stack> buffer;
			
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
			
			void build(string infile, string outfile)
			{
				cout << "Generating new format file in: " << outfile << endl;
				ifstream in(infile.c_str());
				ofstream out(outfile.c_str());
				
				char tab = '\t';
				char end = '\n';
				
				int progress = 0;
				
				while (in.good())
				{
					for (int i=0; i<1000000; ++i)
					{
						if (++progress % 100000 == 0)
						{
							cout << ' ' << progress << '\r' << flush;
						}						
						Stack s;
						
						if (!s.load(in))
						{
							break;
						}
						if ((s.fref == 'C' && s.fcall == 'C') || (s.rref == 'C' && s.rcall == 'C'))
						{
							if (s.fa + s.ft + s.fc + s.fg > s.ftotal)
							{
								s.ftotal = s.fa + s.ft + s.fc + s.fg;
							}
							if (s.ra + s.rt + s.rc + s.rg > s.rtotal)
							{
								s.rtotal = s.ra + s.rt + s.rc + s.rg;
							}
							buffer.push(s);
						}
						else
						{
							--i;
						}
					}
					while (!buffer.empty())
					{
						Stack s = buffer.front();
						if (s.fref == 'C')
						{
							out << s.assembly
								<< tab << s.position
								<< tab << '+'
								<< tab << context(s.assembly, s.position, true)
								<< tab << s.fc
								<< tab << s.ftotal
							<< end;
						}
						else
						{
							out << s.assembly
								<< tab << s.position
								<< tab << '-'
								<< tab << context(s.assembly, s.position, false)
								<< tab << s.rc
								<< tab << s.rtotal
							<< end;
						}
						buffer.pop();
					}
				}
				cout << " " << progress << endl;
				out.close();
				in.close();
			}
			
			//Determine the methylation context
			string context(string assembly, int position, bool forward)
			{
				if (forward)
				{
					if (position+2 >= genome[assembly].size()) return "CG";
					else if (genome[assembly][position+1] == 'G') return "CG";
					else if (genome[assembly][position+2] == 'G') return "CHG";
					else return "CHH";
				}
				else
				{
					if (position-2 < 0) return "CG";
					else if (genome[assembly][position-1] == 'C') return "CG";
					else if (genome[assembly][position-2] == 'C') return "CHG";
					else return "CHH";
				}
			}
		};
	}
}
#endif