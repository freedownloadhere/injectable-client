#pragma once

#include <exception>
#include <string>

class HooksException : std::exception {
public:
	HooksException(const std::string& reason = "Reason not specified")
		: m_reason(reason) { }

	const char* what() const throw() override {
		return m_reason.c_str();
	}

private:
	std::string m_reason;
};
