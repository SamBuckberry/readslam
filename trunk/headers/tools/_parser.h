/**
 * _parser.h
 * 
 * Routines for converting a file of reads into .slam format.
 * Routines for converting a file of reads from .slam format.
 *
 */
#pragma once

#include "../common/_files.h"
#include "../core/_read.h"

#include "../parsing/_annoj.h"
#include "../parsing/_bowtie.h"
#include "../parsing/_fastq.h"
#include "../parsing/_simple.h"
#include "../parsing/_final.h"

namespace ReadSlam
{
	//A collection of functions for converting the format of a file of deep-sequencing reads
	struct Parser
	{
		//Single point controller. Specify the conversion type.
		static void parse(string type, string infile, string outfile)
		{
			if (type == "slam2raw")
			{
				to_raw(infile, outfile);
			}
			else if (type == "slam2fastq")
			{
				to_fastq(infile, outfile);
			}
			else if (type == "slam2annoj")
			{
				to_annoj(infile, outfile);
			}
			else if (type == "slam2bowtie")
			{
				to_bowtie(infile, outfile);
			}
			else if (type == "slam2final")
			{
				to_final(infile, outfile);
			}
			else if (type == "raw2slam")
			{
				from_raw(infile, outfile);
			}
			else if (type == "fastq2slam")
			{
				from_fastq(infile, outfile);
			}
			else if (type == "annoj2slam")
			{
				from_annoj(infile, outfile);
			}
			else if (type == "bowtie2slam")
			{
				from_bowtie(infile, outfile);
			}
			else if (type == "final2slam")
			{
				from_final(infile, outfile);
			}
			else
			{
				cerr << "Unrecognized conversion: " << type << endl;
				exit(1);
			}
		}
		
		//Converters from ReadSlam to other formats
		static void to_raw(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
		
			Read read;
			ReadSimple raw;
		
			while (read.load(in))
			{
				raw.from_slam(read);
				raw.save(out);
			}
			out.close();
			in.close();
		}
	
		static void to_fastq(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
		
			Read read;
			ReadFastQ fastq;
		
			while (read.load(in))
			{
				fastq.from_slam(read);
				fastq.save(out);
			}
			out.close();
			in.close();
		}
	
		static void to_annoj(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
		
			Read read;
			ReadAnnoJ annoj;
		
			while (read.load(in))
			{
				annoj.from_slam(read);
				annoj.save(out);
			}
			out.close();
			in.close();
		}
	
		static void to_bowtie(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
		
			Read read;
			ReadBowtie bowtie;
		
			while (read.load(in))
			{
				bowtie.from_slam(read);
				bowtie.save(out);
			}
			out.close();
			in.close();
		}
		
		static void to_final(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
		
			Read read;
			ReadFinal rf;
		
			while (read.load(in))
			{
				rf.from_slam(read);
				rf.save(out);
			}
			out.close();
			in.close();
		}		
		
		//Convert to ReadSlam from other formats
		static void from_raw(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
			
			ReadSimple raw;
			Read read;
			
			while (raw.load(in))
			{
				raw.to_slam(read);
				read.save(out);
			}
			out.close();
			in.close();
		}
	
		static void from_fastq(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
			
			ReadFastQ fastq;
			Read read;
			
			while (fastq.load(in))
			{
				fastq.to_slam(read);
				read.save(out);
			}
			out.close();
			in.close();
		}
	
		static void from_annoj(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
			
			ReadAnnoJ annoj;
			Read read;
			
			while (annoj.load(in))
			{
				annoj.to_slam(read);
				read.save(out);
			}
			out.close();
			in.close();
		}
	
		static void from_bowtie(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
			
			ReadBowtie bowtie;
			Read read;
			
			while (bowtie.load(in))
			{
				bowtie.to_slam(read);
				read.save(out);
			}
			out.close();
			in.close();
		}
		
		static void from_final(string infile, string outfile)
		{
			ifstream in(infile.c_str());
			ofstream out(outfile.c_str());
			
			ReadFinal rf;
			Read read;
			
			while (rf.load(in))
			{
				rf.to_slam(read);
				read.save(out);
			}
			out.close();
			in.close();
		}
	};
}
