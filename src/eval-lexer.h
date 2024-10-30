#pragma once

#include <istream>
#include <string>
#include <variant>
#include <vector>

enum class EvalTokenType {
	FUNCTION_CALL_START, // '('
	FUNCTION_CALL_END,   // ')'
	INDEXING_START,      // '['
	INDEXING_END,        // ']'
	IDENTIFIER,          // 'a', 'b', 'max'
	NUMBER,              // '1'
	COMMA,               // ','
	DOT                  // '.'
};

struct EvalToken {
	EvalTokenType type;
	std::variant<std::monostate, std::string, double> value;
};

std::vector<EvalToken> tokenizeEval(std::istream &evalStream);
