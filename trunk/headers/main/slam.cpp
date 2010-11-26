/*
 * Single point for running the entire pipeline. Takes a JSON config file as input
 */
#include "../tools/_cli.h"
#include "../core/_json.h"

int main (int argc, char* const argv[])
{
	//Show help on wrong param count
	if (argc != 2)
	{
		cout << "\n"
		<< "\n:: ReadSlam Pipeline ::"
		<< "\nTakes a JSON config file as input and runs the entire ReadSlam pipeline."
		<< "\nSee the template.json file in the /slam directory for an example."
		<< "\nDocumentation available at http://www.jsrev.org/readslam/"
		<< endl;
		exit(0);
	}
	
	//Todo: wrap in exception handler
	ReadSlam::Pipeline slam;
	slam.load_config(argv[1]);
	slam.run();

	return 0;
}
