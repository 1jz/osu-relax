#pragma once

#include <exception>
#include <string>

class Error : public std::exception {
	int errorCode;
	std::string errorMessage;

public:
	Error(int code, std::string message) {
		errorCode = code;
		errorMessage = message;
	}

	const int code() {
		return errorCode;
	}

	virtual const char* what() const noexcept {
		return errorMessage.c_str();
	}
};