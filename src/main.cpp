#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eval-lexer.h"
#include "json-lexer.h"
#include "json-parser.h"

void printJson(const JsonAstNode &ast, std::ostream &outputStream) {
	switch (ast.type) {
	case JsonAstNodeType::OBJECT:
		outputStream << "{ ";
		break;
	case JsonAstNodeType::ARRAY:
		outputStream << "[ ";
		break;
	case JsonAstNodeType::STRING:
		outputStream << "\"" << std::get<std::string>(ast.data) << "\"";
		break;
	case JsonAstNodeType::NUMBER:
		outputStream << std::get<double>(ast.data);
		break;
	case JsonAstNodeType::BOOLEAN_TRUE:
		outputStream << "true";
		break;
	case JsonAstNodeType::BOOLEAN_FALSE:
		outputStream << "false";
		break;
	case JsonAstNodeType::OBJECT_KEY:
		outputStream << "\"" << std::get<std::string>(ast.data) << "\": ";
		break;
	case JsonAstNodeType::NULL_OBJECT:
		outputStream << "null";
		break;
	}

	for (unsigned int i = 0; i < ast.children.size(); i++) {
		if (i > 0) {
			outputStream << ", ";
		}
		printJson(ast.children[i], outputStream);
	}

	switch (ast.type) {
	case JsonAstNodeType::OBJECT:
		outputStream << " }";
		break;
	case JsonAstNodeType::ARRAY:
		outputStream << " ]";
		break;
	}
}

int main() {
	try {
		std::vector<JsonToken> tokens = tokenizeJson(std::cin);
		JsonAstNode jsonAst = parseJsonTokens(tokens);
		printJson(jsonAst, std::cout);
	} catch (char const *errText) {
		std::cerr << errText << std::endl;
		return 1;
	}

	return 0;
}
