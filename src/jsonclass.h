#pragma once

#include <memory>
#include <vector>

enum JsonPushCharStatus {
	SUCCESS,
	FAILURE
};

class JsonObject {
public:
	virtual int pushChar(char c);
	virtual void echo();
};

class JsonObjectHolder {
public:
	std::unique_ptr<JsonObject> obj = std::make_unique<JsonObject>();

	int pushChar(char c);
};

class JsonArray : public JsonObject {
public:
	std::vector<JsonObjectHolder> arr;

	void echo() override;
};
