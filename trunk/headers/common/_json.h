#pragma once

#include "_files.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

using namespace json;

namespace JSON
{
	//Load from file
	static Object load(string filename)
	{
		ifstream in(filename.c_str());
		Object obj;
		Reader::Read(obj, in);
		return obj;
	}
	
	//Write to file
	static void save(Object obj, string filename)
	{
		ofstream out(filename.c_str());
		Writer::Write(obj, out);
	}
}
