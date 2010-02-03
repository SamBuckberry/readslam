#include "_files.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

namespace JSON
{
	//Load from file
	static json::Object load(string filename)
	{
		ifstream in(filename.c_str());
		json::Object obj;
		json::Reader::Read(obj, in);
		return obj;
	}
	
	//Write to file
	static void save(json::Object obj, string filename)
	{
		ofstream out(filename.c_str());
		json::Writer::Write(obj, out);
	}
}

int main()
{
	json::Object obj = JSON::load("/tmp/test1.json");
	JSON::save(obj, "/tmp/test2.json");
	return 0;
}


