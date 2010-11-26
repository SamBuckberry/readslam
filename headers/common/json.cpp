#include "_json.h"

int main()
{
	json::Object obj = JSON::load("/tmp/test1.json");
	obj["age"] = json::Number(32);
	obj["field"] = json::Boolean(true);
	JSON::save(obj, "/tmp/test2.json");
	return 0;
}
