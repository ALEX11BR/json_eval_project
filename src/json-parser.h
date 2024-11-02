#pragma once

#include <string>
#include <variant>
#include <vector>

#include "json-lexer.h"

enum class JsonAstNodeAction {
	GOTO_CHILD,  // A new AST child node was added, go to it
	GOTO_PARENT, // The AST node desn't accept tokens anymore, go to its parent
	STAY         // Keep processing the current AST node in stack
};

enum class JsonAstNodeType {
	ROOT,
	OBJECT,
	ARRAY,
	STRING,
	NUMBER,
	BOOLEAN_TRUE,
	BOOLEAN_FALSE,
	OBJECT_KEY,
	NULL_OBJECT
};

struct JsonAstNode {
	JsonAstNodeType type;
	std::variant<std::monostate, std::string, double> data;
	std::vector<JsonAstNode> children;
	bool inInitialState = true;

	JsonAstNodeAction acceptToken(const JsonToken &token);
	bool acceptsChildren() const;
	bool isSkipNode() const;
};

JsonAstNode parseJsonTokens(const std::vector<JsonToken> &tokens);
