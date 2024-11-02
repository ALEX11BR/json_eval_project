#pragma once

#include <string>
#include <variant>
#include <vector>

#include "eval-lexer.h"

enum class EvalAstNodeAction {
	GOTO_CHILD,
	GOTO_PARENT,
	STAY,
	UPTO_ARRAY_INDEX,
	UPTO_FUNCTION_CALL,
};

enum class EvalAstNodeType {
	ROOT,
	IDENTIFIER,
	FUNCTION_CALL,
	OBJECT_INDEX,
	ARRAY_INDEX,
	NUMBER
};

struct EvalAstNode {
	EvalAstNodeType type;
	std::variant<std::monostate, std::string, double> value;
	std::vector<EvalAstNode> children;

	EvalAstNodeAction acceptToken(const EvalToken &token);
};

EvalAstNode parseEvalTokens(const std::vector<EvalToken> &tokens);
