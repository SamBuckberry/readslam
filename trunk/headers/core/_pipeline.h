#pragma once

#include "../common/_json.h"
#include "index_vector.h"


namespace ReadSlam
{
	//The entire ReadSlam pipeline
	struct Pipeline
	{
		Genome genome;
		Reads reads;
		Stacks stacks;
		Object config;
	
		 Pipeline() { clear(); }
		~Pipeline() { clear(); }
		
		void clear()
		{
			genome.clear();
			reads.clear();
			stacks.clear();
		}

		//Load the pipeline configuration from a JSON configuration file
		void load_config(string infile)
		{
			//TODO: lots of user error and file checking should be done here
			config = JSON::load(infile);
		}
		
		//Run the pipeline
		void run()
		{
			//STEP ONE
			
				//Loop through reads
			
					//Convert read to .slam format
				
					//Trim reads on quality and adapter

					//Drop very poor reads
			
					
					
					
			//STEP TWO

				//Sort the reads by sequence

				//Collapse reads by sequence (keep highest quality candidate)
			
			//STEP THREE
			
				//Load the genome
			
				//Index the genome
			
				//Loop through reads
				
					//Map read
					
					//Filter read
			
			//STEP FOUR
			
				//Sort reads by mapped location
			
				//Collapse reads to remove clones (keep highest quality candidate)
				
			//STEP FIVE
			
				//Load genome
				
				//Build stacks
				
				//Loop through reads
				
					//Add to stacks
					
				//Make calls
				
				//Write stacks
				
				//Write consensus
		}
	};
}
