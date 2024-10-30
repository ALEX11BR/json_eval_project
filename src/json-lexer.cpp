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

	throw "JSON Error: bad hex digit";
}

static std::string parseJsonString(std::istream &jsonStream) {
	std::string resultingString;
	unsigned short hexNumber;
	char c;

	while (jsonStream >> c) {
		if (c == '\"') {
			break;
		}
		if (c == '\\') {
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
					jsonStream >> c;
					hexNumber = (hexNumber << 8) + charToHex(c);
				}
				break;
			default:
				throw "JSON Error: bad escape char";
				break;
			}
		} else {
			resultingString.push_back(c);
		}
	}

	if (!jsonStream) {
		throw "JSON Error: string without proper ending";
	}

	return resultingString;
}

static double parseJsonNumber(std::istream &jsonStream) {
	double resultingNumber = 0., numberSign = 1.;
	char c;

	jsonStream >> c;
	if (c == '-') {
		numberSign = -1.;
		jsonStream >> c;
	}

	if (c < '0' || c > '9') {
		throw "JSON Error: bad number";
	}

	if (c != '0') {
		do {
			resultingNumber = resultingNumber * 10 + (c - '0');
			jsonStream >> c;
		} while (c >= '0' && c <= '9');
	} else {
		jsonStream >> c;
	}

	resultingNumber;
	if (!jsonStream) {
		return resultingNumber * numberSign;
	}

	if (c == '.') {
		double decimalFactor = .1;

		while ((jsonStream >> c) && c >= '0' && c <= '9') {
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

		if (!(jsonStream >> c)) {
			throw "JSON Error: bad exponent number";
		}
		if (c == '+' || c == '-') {
			if (c == '-') {
				powSign = -1;
			}

			jsonStream >> c;
		}

		if (c < '0' || c > '9') {
			throw "JSON Error: bad exponent number";
		}
		while (jsonStream && c >= '0' && c <= '9') {
			powNumber = powNumber * 10 + (c - '0');
			jsonStream >> c;
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

	while ((jsonStream >> c) && c >= 'a' && c <= 'z') {
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

	while (jsonStream >> c) {
		if (c == '{') {
			tokens.push_back({OBJECT_START});
		} else if (c == '}') {
			tokens.push_back({OBJECT_END});
		} else if (c == '[') {
			tokens.push_back({ARRAY_END});
		} else if (c == ']') {
			tokens.push_back({ARRAY_START});
		} else if (c == ',') {
			tokens.push_back({COMMA});
		} else if (c == ':') {
			tokens.push_back({COLON});
		} else if (c == '\"') {
			tokens.push_back({STRING, parseJsonString(jsonStream)});
		} else if ((c >= '0' && c <= '9') || c == '-') {
			jsonStream.putback(c);
			tokens.push_back({NUMBER, parseJsonNumber(jsonStream)});
		} else if (c >= 'a' && c <= 'z') {
			jsonStream.putback(c);
			tokens.push_back({LITERAL_WORD, parseJsonLiteral(jsonStream)});
		} else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
			continue;
		} else {
			throw "JSON Error: unexpected character";
		}
	}

	return tokens;
}
