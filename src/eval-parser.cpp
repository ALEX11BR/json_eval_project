#include "eval-parser.h"

#include <iostream>
#include "print.h"

#include "app-exception.h"

static EvalAstNode tokenToItemNode(const EvalToken &token) {
	switch (token.type) {
	case EvalTokenType::IDENTIFIER:
		return {EvalAstNodeType::IDENTIFIER, std::get<std::string>(token.value)};
	case EvalTokenType::NUMBER:
		return {EvalAstNodeType::NUMBER, std::get<double>(token.value)};
	}
	throw AppException("Expected an item");
}

static EvalAstNode tokenToItemFunction(const EvalToken &token) {
	switch (token.type) {
	case EvalTokenType::DOT:
		return {EvalAstNodeType::OBJECT_INDEX};
	case EvalTokenType::INDEXING_START:
		return {EvalAstNodeType::ARRAY_INDEX};
	case EvalTokenType::FUNCTION_CALL_START:
		return {EvalAstNodeType::FUNCTION_CALL, 0.};
	}
	throw AppException("Expected an operator");
}

EvalAstNodeAction EvalAstNode::acceptToken(const EvalToken &token) {
	switch (type) {
	case EvalAstNodeType::ROOT:
		if (children.empty()) {
			children.push_back(tokenToItemNode(token));
			return EvalAstNodeAction::GOTO_CHILD;
		}
		throw AppException("Unexpected item");
		break;
	case EvalAstNodeType::IDENTIFIER:
		switch (token.type) {
		case EvalTokenType::FUNCTION_CALL_END:
		case EvalTokenType::COMMA:
			return EvalAstNodeAction::UPTO_FUNCTION_CALL;
		case EvalTokenType::INDEXING_END:
			return EvalAstNodeAction::UPTO_ARRAY_INDEX;
		}

		{
			EvalAstNode acceptedFunction = tokenToItemFunction(token);
			if (!children.empty() && acceptedFunction.type == EvalAstNodeType::FUNCTION_CALL) {
				throw AppException("Unexpected function call");
			}
			children.push_back(acceptedFunction);
		}
		return EvalAstNodeAction::GOTO_CHILD;
	case EvalAstNodeType::FUNCTION_CALL:
		if (token.type == EvalTokenType::FUNCTION_CALL_END) {
			return EvalAstNodeAction::GOTO_PARENT;
		}
		if (token.type == EvalTokenType::COMMA) {
			if (std::get<double>(value) == 0.) {
				throw AppException("Unexpected comma in function call");
			}
			value = 0.;
			return EvalAstNodeAction::STAY;
		}
		if (std::get<double>(value) == 1.) {
			throw AppException("Expected comma to separate function arguments");
		}
		value = 1.;
		children.push_back(tokenToItemNode(token));
		return EvalAstNodeAction::GOTO_CHILD;
	case EvalAstNodeType::OBJECT_INDEX:
		if (token.type != EvalTokenType::IDENTIFIER) {
			throw AppException("Unexpected key");
		}
		children.push_back(tokenToItemNode(token));
		return EvalAstNodeAction::GOTO_CHILD;
	case EvalAstNodeType::ARRAY_INDEX:
		if (token.type == EvalTokenType::INDEXING_END) {
			if (children.empty()) {
				throw AppException("Empty array indexing is not allowed");
			}
			return EvalAstNodeAction::GOTO_PARENT;
		}
		children.push_back(tokenToItemNode(token));
		return EvalAstNodeAction::GOTO_CHILD;
	case EvalAstNodeType::NUMBER:
		switch (token.type) {
		case EvalTokenType::FUNCTION_CALL_END:
		case EvalTokenType::COMMA:
			return EvalAstNodeAction::UPTO_FUNCTION_CALL;
		case EvalTokenType::INDEXING_END:
			return EvalAstNodeAction::UPTO_ARRAY_INDEX;
		}
		throw AppException("Unexpected operation on a number");
		break;
	}
	throw AppException("Unexpected situation");
}

EvalAstNode parseEvalTokens(const std::vector<EvalToken> &tokens) {
	EvalAstNode rootNode = {EvalAstNodeType::ROOT};
	std::vector<EvalAstNode *> nodeStack = {&rootNode};

	for (const auto &token : tokens) {
		bool keepRunning = false;
		do {
			keepRunning = false;
			EvalAstNodeAction action = nodeStack.back()->acceptToken(token);
			switch (action) {
			case EvalAstNodeAction::GOTO_CHILD:
				nodeStack.push_back(&(nodeStack.back()->children.back()));
				break;
			case EvalAstNodeAction::GOTO_PARENT:
				nodeStack.pop_back();
				break;
			case EvalAstNodeAction::UPTO_ARRAY_INDEX:
				while (nodeStack.back()->type != EvalAstNodeType::ARRAY_INDEX) {
					if (nodeStack.back()->type == EvalAstNodeType::FUNCTION_CALL ||
					    nodeStack.back()->type == EvalAstNodeType::ROOT) {
						throw AppException("Unexpected array index end");
					}
					nodeStack.pop_back();
				}
				keepRunning = true;
				break;
			case EvalAstNodeAction::UPTO_FUNCTION_CALL:
				while (nodeStack.back()->type != EvalAstNodeType::FUNCTION_CALL) {
					if (nodeStack.back()->type == EvalAstNodeType::ARRAY_INDEX ||
					    nodeStack.back()->type == EvalAstNodeType::ROOT) {
						throw AppException("Unexpected function call end");
					}
					nodeStack.pop_back();
				}
				keepRunning = true;
				break;
			}
		} while (keepRunning);
	}

	if (nodeStack.back()->type == EvalAstNodeType::OBJECT_INDEX) {
		throw AppException("Unfinished object index");
	}
	while (!nodeStack.empty()) {
		if (nodeStack.back()->type == EvalAstNodeType::FUNCTION_CALL ||
		    nodeStack.back()->type == EvalAstNodeType::ARRAY_INDEX) {
			throw AppException("Unclosed bracket");
		}
		nodeStack.pop_back();
	}

	if (rootNode.children.empty()) {
		throw AppException("Nothing specified");
	}
	return std::move(rootNode.children.front());
}
