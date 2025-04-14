#pragma once
#include "pch.hpp"

class JavaException : public std::exception {
public:
	JavaException(const std::string& reason = "Reason not specified")
		: m_reason(reason) { }

	const char* what() const throw() override {
		return m_reason.c_str();
	}

private:
	std::string m_reason;
};
