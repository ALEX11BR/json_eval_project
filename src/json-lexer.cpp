#include "json-lexer.h"

#include <cmath>

static unsigned char charToHex(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'A' && c <= 'F') {
		return 10 + (c - 'A');
	}
	if (c >= 'a' && c <= 'f') {
		return 10 + (c - 'a');
	}

	throw "bad hex digit";
}

static std::string parseJsonString(std::istream &jsonStream) {
	std::string resultingString;
	unsigned short hexNumber;
	char c;

	while (jsonStream.get(c)) {
		if (c == '\"') {
			break;
		}
		if (c == '\\') {
			jsonStream.get(c);

			switch (c) {
			case '\"':
			case '\\':
			case '/':
				resultingString.push_back(c);
				break;
			case 'b':
				resultingString.push_back('\b');
				break;
			case 'f':
				resultingString.push_back('\f');
				break;
			case 'n':
				resultingString.push_back('\n');
				break;
			case 'r':
				resultingString.push_back('\r');
				break;
			case 't':
				resultingString.push_back('\t');
				break;
			case 'u':
				hexNumber = 0;
				for (int i = 0; i < 4; i++) {
					jsonStream.get(c);
					hexNumber = (hexNumber << 4) + charToHex(c);
				}
				if (hexNumber >= 0x80) {
					throw "Non-ASCII hex string escapes are not supported.";
				}
				resultingString.push_back((char)hexNumber);
				break;
			default:
				throw "bad escape char";
				break;
			}
		} else {
			resultingString.push_back(c);
		}
	}

	if (!jsonStream) {
		throw "string without proper ending";
	}

	return resultingString;
}

static double parseEvalNumber(std::istream &jsonStream) {
	double resultingNumber = 0., numberSign = 1.;
	char c;

	jsonStream.get(c);
	if (c == '-') {
		numberSign = -1.;
		jsonStream.get(c);
	}

	if (c < '0' || c > '9') {
		throw "bad number";
	}

	if (c != '0') {
		do {
			resultingNumber = resultingNumber * 10 + (c - '0');
		} while (jsonStream.get(c) && c >= '0' && c <= '9');
	} else {
		jsonStream.get(c);
	}

	if (!jsonStream) {
		return resultingNumber * numberSign;
	}

	if (c == '.') {
		double decimalFactor = .1;

		while ((jsonStream.get(c)) && c >= '0' && c <= '9') {
			resultingNumber += decimalFactor * (c - '0');
			decimalFactor /= 10;
		}
	}
	resultingNumber *= numberSign;
	if (!jsonStream) {
		return resultingNumber;
	}

	if (c == 'E' || c == 'e') {
		long powNumber = 0, powSign = 1;

		if (!(jsonStream.get(c))) {
			throw "bad exponent number";
		}
		if (c == '+' || c == '-') {
			if (c == '-') {
				powSign = -1;
			}

			jsonStream.get(c);
		}

		if (c < '0' || c > '9') {
			throw "bad exponent number";
		}
		while (jsonStream && c >= '0' && c <= '9') {
			powNumber = powNumber * 10 + (c - '0');
			jsonStream.get(c);
		}
		resultingNumber = resultingNumber * std::pow(10, powNumber * powSign);
	}
	if (jsonStream) {
		jsonStream.putback(c);
	}

	return resultingNumber;
}

static std::string parseJsonLiteral(std::istream &jsonStream) {
	std::string resultingLiteral;
	char c;

	while ((jsonStream.get(c)) && c >= 'a' && c <= 'z') {
		resultingLiteral.push_back(c);
	}
	if (jsonStream) {
		jsonStream.putback(c);
	}

	return resultingLiteral;
}

std::vector<JsonToken> tokenizeJson(std::istream &jsonStream) {
	std::vector<JsonToken> tokens;
	char c;

	while (jsonStream.get(c)) {
		if (c == '{') {
			tokens.push_back({JsonTokenType::OBJECT_START});
		} else if (c == '}') {
			tokens.push_back({JsonTokenType::OBJECT_END});
		} else if (c == '[') {
			tokens.push_back({JsonTokenType::ARRAY_START});
		} else if (c == ']') {
			tokens.push_back({JsonTokenType::ARRAY_END});
		} else if (c == ',') {
			tokens.push_back({JsonTokenType::COMMA});
		} else if (c == ':') {
			tokens.push_back({JsonTokenType::COLON});
		} else if (c == '\"') {
			tokens.push_back({JsonTokenType::STRING, parseJsonString(jsonStream)});
		} else if ((c >= '0' && c <= '9') || c == '-') {
			jsonStream.putback(c);
			tokens.push_back({JsonTokenType::NUMBER, parseEvalNumber(jsonStream)});
		} else if (c >= 'a' && c <= 'z') {
			jsonStream.putback(c);
			tokens.push_back({JsonTokenType::LITERAL_WORD, parseJsonLiteral(jsonStream)});
		} else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
			continue;
		} else {
			throw "unexpected character";
		}
	}

	return tokens;
}
