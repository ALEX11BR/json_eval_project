#include "print.h"

#include <iostream>

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

void debugEval(const EvalAstNode &ast, int level = 0) {
	for (int i = 0; i < level; i++) {
		std::cout << '>';
	}

	std::cout << (int)ast.type;

	try {
		std::cout << " " << std::get<std::string>(ast.value);
	} catch (...) {
	}
	try {
		std::cout << " " << std::get<double>(ast.value);
	} catch (...) {
	}
	std::cout << "\n";

	for (const auto &c : ast.children) {
		debugEval(c, level + 1);
	}
}
