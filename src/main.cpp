#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eval-lexer.h"
#include "json-lexer.h"

int main() {
	try {
		std::vector<EvalToken> tokens = tokenizeEval(std::cin);
		for (const auto &t : tokens) {
			std::cout << static_cast<int>(t.type) << " ";
			if (t.type == EvalTokenType::NUMBER) {
				std::cout << std::get<double>(t.value);
			}
			if (t.type == EvalTokenType::IDENTIFIER) {
				std::cout << std::get<std::string>(t.value);
			}
			std::cout << std::endl;
		}
	} catch (char const *errText) {
		std::cerr << errText << std::endl;
		return 1;
	}

	return 0;
}
