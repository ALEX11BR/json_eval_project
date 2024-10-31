#pragma once

#include <string>
#include <variant>
#include <vector>

#include "common-parser.h"
#include "json-lexer.h"

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

	AstNodeAcceptStatus acceptToken(const JsonToken &token);
	bool acceptsChildren() const;
	bool isSkipNode() const;
};

JsonAstNode parseJsonTokens(const std::vector<JsonToken> &tokens);
