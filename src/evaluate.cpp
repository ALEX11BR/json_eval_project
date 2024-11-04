#include "evaluate.h"

#include <algorithm>
#include <numeric>
#include <vector>

#include "app-exception.h"

JsonAstNode evaluate(const EvalAstNode &evalAst, const JsonAstNode &jsonAst, const JsonAstNode &jsonRoot) {
	switch (evalAst.type) {
	case EvalAstNodeType::NUMBER:
		return {JsonAstNodeType::NUMBER, std::get<double>(evalAst.value)};
	case EvalAstNodeType::IDENTIFIER:
		if (evalAst.children.size() == 1 && evalAst.children.front().type == EvalAstNodeType::FUNCTION_CALL) {
			std::vector<JsonAstNode> arguments;
			for (const auto &argNode : evalAst.children.front().children) {
				arguments.push_back(evaluate(argNode, jsonRoot, jsonRoot));
			}
			if (std::get<std::string>(evalAst.value) == "size") {
				if (arguments.size() != 1) {
					throw AppException("Invalid arguments for function 'size'");
				}
				switch (arguments.front().type) {
				case JsonAstNodeType::ARRAY:
				case JsonAstNodeType::OBJECT:
					return {JsonAstNodeType::NUMBER, (double)arguments.front().children.size()};
				case JsonAstNodeType::STRING:
					return {JsonAstNodeType::NUMBER, (double)std::get<std::string>(arguments.front().data).size()};
				}
			} else {
				if (arguments.size() == 1) {
					if (arguments[0].type == JsonAstNodeType::ARRAY) {
						arguments = arguments[0].children;
					}
				}
				if (arguments.empty()) {
					throw AppException("Function can't work with no arguments");
				}

				if (std::get<std::string>(evalAst.value) == "max") {
					double ans = std::get<double>(arguments[0].data);
					for (unsigned int i = 1; i < arguments.size(); i++) {
						ans = std::max(ans, std::get<double>(arguments[i].data));
					}

					return {JsonAstNodeType::NUMBER, ans};
				}
				if (std::get<std::string>(evalAst.value) == "min") {
					double ans = std::get<double>(arguments[0].data);
					for (unsigned int i = 1; i < arguments.size(); i++) {
						ans = std::min(ans, std::get<double>(arguments[i].data));
					}

					return {JsonAstNodeType::NUMBER, ans};
				}
				if (std::get<std::string>(evalAst.value) == "sum") {
					double ans = 0;
					for (const auto &arg : arguments) {
						ans += std::get<double>(arg.data);
					}

					return {JsonAstNodeType::NUMBER, ans};
				}
			}
			throw AppException("Invalid function");
		}

		if (jsonAst.type != JsonAstNodeType::OBJECT) {
			throw AppException("Can't access object from key");
		}

		auto object = std::lower_bound(
		    jsonAst.children.begin(), jsonAst.children.end(), std::get<std::string>(evalAst.value),
		    [](const JsonAstNode &node, const std::string &str) { return std::get<std::string>(node.data) < str; });
		if (object == jsonAst.children.end() || object->data != evalAst.value) {
			throw AppException("Object does not have the specified index");
		}

		if (evalAst.children.empty()) {
			return object->children[0];
		}
		auto obj = object->children[0];

		for (const auto &operation : evalAst.children) {
			if (operation.type == EvalAstNodeType::ARRAY_INDEX) {
				if (obj.type != JsonAstNodeType::ARRAY) {
					throw AppException("Can't do an array index from a non-array");
				}

				JsonAstNode indexNode = evaluate(evalAst.children.front().children[0], jsonRoot, jsonRoot);
				if (indexNode.type != JsonAstNodeType::NUMBER) {
					throw AppException("Bad array index type");
				}
				int index = std::get<double>(indexNode.data);
				if (index < 0 || index >= (int)obj.children.size()) {
					throw AppException("Array index out of bounds");
				}

				obj = obj.children[index];
			} else if (operation.type == EvalAstNodeType::OBJECT_INDEX) {
				obj = evaluate(operation.children[0], obj, jsonRoot);
			} else {
				throw AppException("Unsupported operation");
			}
		}
		return obj;
	}
	throw AppException("Invalid eval type");
}
