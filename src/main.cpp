#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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
	} catch (char const *errText) {
		std::cerr << "Eval tokenization error: " << errText << std::endl;
		std::exit(1);
	}
	try {
		evalAst.set_value(parseEvalTokens(evalTokens));
	} catch (char const *errText) {
		std::cerr << "Eval parsing error: " << errText << std::endl;
		std::exit(1);
	}
}

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: ./json_eval file.json 'string[2].eval'\n";
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
	} catch (char const *errText) {
		std::cerr << "Json tokenization error: " << errText << std::endl;
		return 1;
	}
	try {
		jsonAst = parseJsonTokens(jsonTokens);
	} catch (char const *errText) {
		std::cerr << "Json parsing error: " << errText << std::endl;
		return 1;
	}
	evalThread.join();

	EvalAstNode evalAst = evalAstFuture.get();

	try {
		printJson(evaluate(evalAst, jsonAst, jsonAst), std::cout);
		std::cout << '\n';
	} catch (char const *errText) {
		std::cerr << "Evaluation error: " << errText << std::endl;
		return 1;
	}

	return 0;
}
