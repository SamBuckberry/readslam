/*
 * C++ based management utility for ReadSlam
 *
 * (C) 2009 Julian Tonti-Filippini
 * julian.tonti@gmail.com
 *
 * Disclaimer: you use this entirely at your own risk
 */
#include "../tools/_parser.h"

int main (int argc, char* const argv[])
{
	if (argc != 4)
	{
		cout << "\n"
		<<"\n--------------------------------------------------------------------------------"
		<<"\n"
		<<"\n:: ReadSlam Parser ::"
		<<"\nA collection of routines for parsing to and from .slam format"
		<<"\n"
		<<"\n:: Examples ::"
		<<"\nNote: command keywords are case sensitive"
		<<"\n"
		<<"\n- Parse from .slam to another format"
		<<"\n    ./parser slam2raw ./reads.slam ./reads.raw"
		<<"\n    ./parser slam2fastq ./reads.slam ./reads.fastq"
		<<"\n    ./parser slam2annoj ./reads.slam ./reads.annoj"
		<<"\n    ./parser slam2final ./reads.slam ./reads.final"
		<<"\n    ./parser slam2bowtie ./reads.slam ./reads.bowtie"
		<<"\n"
		<<"\n- Parse from another format to .slam"
		<<"\n    ./parser raw2slam ./read.raw ./reads.slam"
		<<"\n    ./parser fastq2slam ./read.fastq ./reads.slam"
		<<"\n    ./parser annoj2slam ./read.annoj ./reads.slam"
		<<"\n    ./parser final2slam ./reads.final ./reads.slam"
		<<"\n    ./parser bowtie2slam ./read.bowtie ./reads.slam"
		<<"\n"
		<<"\n--------------------------------------------------------------------------------"
		<<"\n"
		<<"\n:: More Information ::"
		<<"\n"
		<<"\n- Website: http://readslam.revjs.org"
		<<"\n- Contact: Julian Tonti-Filippini (julian@revjs.org)"
		<<"\n"
		<<"\n--------------------------------------------------------------------------------"
		<<"\n"
		<< endl;
		
		return 0;
	}
	
	ReadSlam::Parser::parse(argv[1], argv[2], argv[3]);

	return 0;
}
