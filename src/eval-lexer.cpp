#include "eval-lexer.h"

#include <cmath>

static double parseEvalNumber(std::istream &evalStream) {
	double resultingNumber = 0., numberSign = 1.;
	char c;

	evalStream.get(c);
	if (c == '-') {
		numberSign = -1.;
		evalStream.get(c);
	}

	if (c < '0' || c > '9') {
		throw "Eval error: bad number";
	}

	if (c != '0') {
		do {
			resultingNumber = resultingNumber * 10 + (c - '0');
		} while (evalStream.get(c) && c >= '0' && c <= '9');
	} else {
		evalStream.get(c);
	}

	if (!evalStream) {
		return resultingNumber * numberSign;
	}

	if (c == '.') {
		double decimalFactor = .1;

		while ((evalStream.get(c)) && c >= '0' && c <= '9') {
			resultingNumber += decimalFactor * (c - '0');
			decimalFactor /= 10;
		}
	}
	resultingNumber *= numberSign;
	if (!evalStream) {
		return resultingNumber;
	}

	if (c == 'E' || c == 'e') {
		long powNumber = 0, powSign = 1;

		if (!(evalStream.get(c))) {
			throw "Eval string error: bad exponent number";
		}
		if (c == '+' || c == '-') {
			if (c == '-') {
				powSign = -1;
			}

			evalStream.get(c);
		}

		if (c < '0' || c > '9') {
			throw "Eval string error: bad exponent number";
		}
		while (evalStream && c >= '0' && c <= '9') {
			powNumber = powNumber * 10 + (c - '0');
			evalStream.get(c);
		}
		resultingNumber = resultingNumber * std::pow(10, powNumber * powSign);
	}
	if (evalStream) {
		evalStream.putback(c);
	}

	return resultingNumber;
}

static bool isIdentifierLetter(char letter) {
    return (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') || (letter == '_');
}

static std::string parseEvalIdentifier(std::istream &jsonStream) {
	std::string resultingLiteral;
	char c;

	while ((jsonStream.get(c)) && isIdentifierLetter(c)) {
		resultingLiteral.push_back(c);
	}
	if (jsonStream) {
		jsonStream.putback(c);
	}

	return resultingLiteral;
}

std::vector<EvalToken> tokenizeEval(std::istream &evalStream) {
    std::vector<EvalToken> tokens;
    char c;

    while (evalStream.get(c)) {
        if (c == '(') {
            tokens.push_back({EvalTokenType::FUNCTION_CALL_START});
        } else if (c == ')') {
            tokens.push_back({EvalTokenType::FUNCTION_CALL_END});
        } else if (c == '[') {
			tokens.push_back({EvalTokenType::INDEXING_START});
		} else if (c == ']') {
			tokens.push_back({EvalTokenType::INDEXING_END});
		} else if (c == ',') {
			tokens.push_back({EvalTokenType::COMMA});
		} else if (c == '.') {
			tokens.push_back({EvalTokenType::DOT});
		} else if ((c >= '0' && c <= '9') || c == '-') {
			evalStream.putback(c);
			tokens.push_back({EvalTokenType::NUMBER, parseEvalNumber(evalStream)});
		} else if (isIdentifierLetter(c)) {
			evalStream.putback(c);
			tokens.push_back({EvalTokenType::IDENTIFIER, parseEvalIdentifier(evalStream)});
		} else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
			continue;
		} else {
			throw "Eval string error: unexpected character";
		}
    }

    return tokens;
}
