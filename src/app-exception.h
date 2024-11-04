#pragma once

#include <exception>
#include <string>

class AppException : public std::exception {
public:
	AppException(const std::string &str);
	const char *what() const noexcept override;

private:
	std::string text;
};
