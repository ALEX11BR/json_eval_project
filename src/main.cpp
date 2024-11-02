#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "eval-lexer.h"
#include "eval-parser.h"
#include "json-lexer.h"
#include "json-parser.h"
#include "print.h"

int main(int argc, char **argv) {
	std::vector<JsonToken> jsonTokens;
	JsonAstNode jsonAst;
	std::vector<EvalToken> evalTokens;
	EvalAstNode evalAst;

	if (argc != 3) {
		std::cerr << "Usage: ./json_eval file.json 'string[2].eval'\n";
		return 1;
	}

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
	try {
		std::stringstream evalArg(argv[2]);
		evalTokens = tokenizeEval(evalArg);
	} catch (char const *errText) {
		std::cerr << "Eval tokenization error: " << errText << std::endl;
		return 1;
	}
	try {
		evalAst = parseEvalTokens(evalTokens);
	} catch (char const *errText) {
		std::cerr << "Eval parsing error: " << errText << std::endl;
		return 1;
	}

	printJson(jsonAst, std::cout);
	debugEval(evalAst, 0);

	return 0;
}
