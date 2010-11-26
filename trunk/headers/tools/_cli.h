#pragma once

#include "_preprocess.h"
#include "_parser.h"
#include "_sorter.h"
#include "../core/_genome.h"

namespace ReadSlam
{
	struct CLI
	{
		void trim(string infile, string outfile)
		{
			ReadSlam::PreProcessor p;
			p.trim(infile, outfile);
		}
		void map(string genome, string infile, string outfile, int seed, bool bisulfite, bool usemap)
		{
			ReadSlam::Genome g;
			g.load(genome);
			g.build_index(seed,bisulfite,usemap);
			g.map_reads(infile,outfile,true);
			//g.map_threaded(infile,outfile);
		}
		void stack(string genome, string infile, string outfile)
		{
			ReadSlam::Genome g;
			g.load(genome);
			g.stack(infile, outfile);
		}
		void meth(string genome, string infile, string outfile)
		{
			ReadSlam::Genome g;
			g.load(genome);
			g.meth(infile, outfile);
		}
		void parse(string type, string infile, string outfile)
		{
			ReadSlam::Parser::parse(type, infile, outfile);
		}
		void sort_reads(string type, int chunk, string infile, string outfile)
		{
			ReadSlam::Sorter::sort_reads(type,chunk,infile,outfile);
		}
	};
}
