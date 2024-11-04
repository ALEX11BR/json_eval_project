#include "json-parser.h"

#include <algorithm>

#include "app-exception.h"

static JsonAstNode tokenToItemNode(const JsonToken &token) {
	switch (token.type) {
	case JsonTokenType::OBJECT_START:
		return {JsonAstNodeType::OBJECT};
	case JsonTokenType::ARRAY_START:
		return {JsonAstNodeType::ARRAY};
	case JsonTokenType::STRING:
		return {JsonAstNodeType::STRING, std::get<std::string>(token.value)};
	case JsonTokenType::NUMBER:
		return {JsonAstNodeType::NUMBER, std::get<double>(token.value)};
	case JsonTokenType::LITERAL_WORD:
		if (std::get<std::string>(token.value) == "null") {
			return {JsonAstNodeType::NULL_OBJECT};
		}
		if (std::get<std::string>(token.value) == "false") {
			return {JsonAstNodeType::BOOLEAN_FALSE};
		}
		if (std::get<std::string>(token.value) == "true") {
			return {JsonAstNodeType::BOOLEAN_TRUE};
		}
		break;
	}

	throw AppException("Unexpected JSON token");
}

JsonAstNodeAction JsonAstNode::acceptToken(const JsonToken &token) {
	switch (type) {
	case JsonAstNodeType::ROOT:
		if (children.empty()) {
			children.push_back(tokenToItemNode(token));
			return JsonAstNodeAction::GOTO_CHILD;
		}
		throw AppException("Unexpected item");
		break;
	case JsonAstNodeType::ARRAY:
		if (inInitialState) {
			if (token.type == JsonTokenType::ARRAY_END) {
				if (children.empty()) {
					return JsonAstNodeAction::GOTO_PARENT;
				}
				throw AppException("Unexpected array end");
			}
			children.push_back(tokenToItemNode(token));
			inInitialState = false;
			return JsonAstNodeAction::GOTO_CHILD;
		} else {
			if (token.type == JsonTokenType::COMMA) {
				inInitialState = true;
				return JsonAstNodeAction::STAY;
			}
			if (token.type == JsonTokenType::ARRAY_END) {
				return JsonAstNodeAction::GOTO_PARENT;
			}
			throw AppException("Expected comma to separate array elements");
		}
		break;
	case JsonAstNodeType::OBJECT_KEY:
		if (inInitialState) {
			if (token.type != JsonTokenType::COLON) {
				throw AppException("Expected colon to separate values from object keys");
			}
			inInitialState = false;
			return JsonAstNodeAction::STAY;
		} else {
			children.push_back(tokenToItemNode(token));
			return JsonAstNodeAction::GOTO_CHILD;
		}
		break;
	case JsonAstNodeType::OBJECT:
		if (inInitialState) {
			if (token.type == JsonTokenType::OBJECT_END) {
				if (children.empty()) {
					return JsonAstNodeAction::GOTO_PARENT;
				}
				throw AppException("Unexpected object end");
			}
			if (token.type == JsonTokenType::STRING) {
				children.push_back({JsonAstNodeType::OBJECT_KEY, std::get<std::string>(token.value)});
				inInitialState = false;
				return JsonAstNodeAction::GOTO_CHILD;
			}
			throw AppException("Unsupported key");
		} else {
			if (token.type == JsonTokenType::COMMA) {
				inInitialState = true;
				return JsonAstNodeAction::STAY;
			}
			if (token.type == JsonTokenType::OBJECT_END) {
				return JsonAstNodeAction::GOTO_PARENT;
			}
			throw AppException("Expected comma to separate object members");
		}
		break;
	}
	throw AppException("Unexpected parsing situation");
}

bool JsonAstNode::acceptsChildren() const {
	switch (type) {
	case JsonAstNodeType::ROOT:
	case JsonAstNodeType::ARRAY:
	case JsonAstNodeType::OBJECT:
	case JsonAstNodeType::OBJECT_KEY:
		return true;
	}
	return false;
}

bool JsonAstNode::isSkipNode() const {
	return type == JsonAstNodeType::OBJECT_KEY && !inInitialState && !children.empty();
}

JsonAstNode parseJsonTokens(const std::vector<JsonToken> &tokens) {
	JsonAstNode rootNode = {JsonAstNodeType::ROOT};
	std::vector<JsonAstNode *> nodeStack = {&rootNode};

	for (const auto &token : tokens) {
		JsonAstNodeAction action = nodeStack.back()->acceptToken(token);
		if (action == JsonAstNodeAction::GOTO_CHILD) {
			JsonAstNode &latestChild = nodeStack.back()->children.back();
			if (latestChild.acceptsChildren()) {
				nodeStack.push_back(&latestChild);
			}
		} else if (action == JsonAstNodeAction::GOTO_PARENT) {
			if (nodeStack.back()->type == JsonAstNodeType::OBJECT) {
				auto &children = nodeStack.back()->children;
				std::stable_sort(children.begin(), children.end(), [](const JsonAstNode &a, const JsonAstNode &b) {
					return std::get<std::string>(a.data) < std::get<std::string>(b.data);
				});
				for (unsigned int i = 1; i < children.size(); i++) {
					if (children[i].data == children[i - 1].data) {
						children.erase(children.begin() + (i - 1));
						i--;
					}
				}
			}
			nodeStack.pop_back();
		}
		if (nodeStack.back()->isSkipNode()) {
			nodeStack.pop_back();
		}
	}

	if (rootNode.children.empty()) {
		throw AppException("Empty JSON");
	}
	if (nodeStack.size() != 1) {
		throw AppException("Unfinished JSON");
	}
	return std::move(rootNode.children.front());
}
