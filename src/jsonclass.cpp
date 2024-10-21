#include <iostream>

#include "jsonobjects.h"

int JsonObject::pushChar(char c) {
}

void JsonObject::echo() {
	std::cout << "dsfsdn\n";
}
void JsonArray::echo() {
	std::cout << "grojjA\n";
}

int JsonObjectHolder::pushChar(char c) {
	obj = std::make_unique<JsonArray>();

	return 0;
}
