#pragma once

#include <istream>
#include <string>
#include <variant>
#include <vector>

enum JsonTokenType {
	OBJECT_START, // '{'
	OBJECT_END,   // '}'
	ARRAY_START,  //'['
	ARRAY_END,    // ']'
	STRING,       // '"string"'
	NUMBER,       // '1.23'
	LITERAL_WORD, // 'true', 'false', 'null'
	COMMA,        // ','
	COLON         // ':'
};

struct JsonToken {
	JsonTokenType type;
	std::variant<std::string, double> value;
};
