#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "app-exception.h"
#include "eval-lexer.h"
#include "eval-parser.h"
#include "evaluate.h"
#include "json-lexer.h"
#include "json-parser.h"
#include "print.h"

void evalParseThread(std::promise<EvalAstNode> evalAst, const std::string &evalStr) {
	std::vector<EvalToken> evalTokens;

	try {
		std::stringstream evalArg(evalStr);
		evalTokens = tokenizeEval(evalArg);
	} catch (const std::exception &err) {
		std::cerr << "Eval tokenization error: " << err.what() << std::endl;
		std::exit(1);
	}
	try {
		evalAst.set_value(parseEvalTokens(evalTokens));
	} catch (const std::exception &err) {
		std::cerr << "Eval parsing error: " << err.what() << std::endl;
		std::exit(1);
	}
}

int main(int argc, char **argv) {
	if (argc != 4) {
		std::cerr << "Usage: ./test-runner file.json 'string[2].eval' 'expected output'\n";
		return 1;
	}

	std::promise<EvalAstNode> evalAstPromise;
	std::future<EvalAstNode> evalAstFuture = evalAstPromise.get_future();
	std::thread evalThread(evalParseThread, std::move(evalAstPromise), std::string(argv[2]));

	std::vector<JsonToken> jsonTokens;
	JsonAstNode jsonAst;
	try {
		std::ifstream inputFile(argv[1]);
		jsonTokens = tokenizeJson(inputFile);
	} catch (const std::exception &err) {
		std::cerr << "Json tokenization error: " << err.what() << std::endl;
		return 1;
	}
	try {
		jsonAst = parseJsonTokens(jsonTokens);
	} catch (const std::exception &err) {
		std::cerr << "Json parsing error: " << err.what() << std::endl;
		return 1;
	}
	evalThread.join();

	EvalAstNode evalAst = evalAstFuture.get();

	try {
		std::stringstream resultStream;
		printToUser(evaluate(evalAst, jsonAst, jsonAst), resultStream);

		if (resultStream.str() != argv[3]) {
			std::cerr << "Expected " << argv[3] << " but got " << resultStream.str() << std::endl;
			return 2;
		}
	} catch (const std::exception &err) {
		std::cerr << "Evaluation error: " << err.what() << std::endl;
		return 1;
	}

	return 0;
}
