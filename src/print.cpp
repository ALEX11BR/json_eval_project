#include "print.h"

#include <iostream>
#include <variant>

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

void printToUser(const JsonAstNode &ast, std::ostream &outputStream) {
	switch (ast.type) {
	case JsonAstNodeType::STRING:
		outputStream << std::get<std::string>(ast.data);
		break;
	case JsonAstNodeType::NUMBER:
		outputStream << std::get<double>(ast.data);
		break;
	default:
		printJson(ast, outputStream);
		break;
	}
}

void debugEval(const EvalAstNode &ast, std::ostream &outputStream, int level = 0) {
	for (int i = 0; i < level; i++) {
		outputStream << ">";
	}

	outputStream << (int)ast.type;

	const std::string *str = std::get_if<std::string>(&ast.value);
	if (str != NULL) {
		outputStream << " " << *str;
	}

	const double *num = std::get_if<double>(&ast.value);
	if (num != NULL) {
		outputStream << " " << *num;
	}

	outputStream << "\n";

	for (const auto &c : ast.children) {
		debugEval(c, outputStream, level + 1);
	}
}
