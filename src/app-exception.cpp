#include "app-exception.h"

AppException::AppException(const std::string &str) {
	text = str;
}

const char *AppException::what() const noexcept {
	return text.c_str();
}
