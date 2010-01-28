#ifndef _READSLAM_SYSINFO
#define _READSLAM_SYSINFO

#include "_common.h"

/*
 * Simple routines for determining some basic system information
 * Only works on unix systems
 */
namespace ReadSlam
{
	struct Sysinfo
	{
		int cpus;
		int ram;
		
		
		Sysinfo()
		{
			cpus = count_cpus();
			ram = count_ram();
			
			// cout << "Int: " << numeric_limits<int>::max() << endl;
			// cout << "Unsigned int: " << numeric_limits<unsigned int>::max() << endl;
			// cout << "Long: " << numeric_limits<long>::max() << endl;
			// cout << "Unsigned long: " << numeric_limits<unsigned long>::max() << endl;
			
			// cout << "System has " << cpus << " cpus" << endl;
			// cout << "System has " << ram << "MB RAM" << endl;
		}
		~Sysinfo()
		{
			cpus = 0;
			ram = 0;
		}
		
		//Determine how many cpus the system has
		int count_cpus()
		{
			ifstream in("/proc/cpuinfo");
			
			if (!in)
			{
				cerr << "Unable to open '/proc/cpuinfo' defaulting to 1 CPU" << endl;
				return 1;
			}
			
			string token = "";
			int count = 0;

			while (in >> token)
			{
				if (token == "processor")
				{
					count++;
				}
			}

			if (count == 0)
			{
				cerr << "Unable to determine number of CPUs from '/proc/cpuinfo' defaulting to 1 CPU" << endl;
				count = 1;
			}
			return count;
		}

		//Determine how much RAM the system has in MB
		int count_ram()
		{
			ifstream in("/proc/meminfo");
			
			if (!in)
			{
				cerr << "Unable to open '/proc/meminfo' defaulting to 1000 MB RAM" << endl;
				return 1000;
			}

			string token = "";
			
			while (in >> token)
			{
				if (token == "MemTotal:")
				{
					unsigned long mem;
					
					if (in >> mem)
					{
						return mem / 1000;
					}
					else
					{
						cerr << "Unable to determine available RAM, defaulting to 1000 MB" << endl;
						return 1000;
					}
				}
			}
			cerr << "Unable to determine available RAM, defaulting to 1000 MB" << endl;
			return 1000;
		}
	};
}
#endif