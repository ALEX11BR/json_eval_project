#pragma once

#include <map>
#include <vector>

enum JsonObjectType {
	JsonNONE,
	JsonDICT,
	JsonARR,
	JsonSTR,
	JsonNUM,
	JsonBOOL,
	JsonNULL
};

struct JsonDictData {
	std::map<std::string, JsonObject> dict;
};
struct JsonArrData {
	std::vector<JsonObject> dict;
};

union JsonObjectData {
	JsonDictData dict;
};

struct JsonObject {
	JsonObjectType type = JsonNONE;
	JsonObjectData data;

	std::string temp;
};
