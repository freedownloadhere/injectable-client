#pragma once
#include "pch.hpp"

class Exception : public std::exception {
public:
	Exception(const std::string& reason = "Reason not specified")
		: m_reason(reason) { }

	virtual const char* what() const throw() override {
		return m_reason.c_str();
	}

	virtual ~Exception() = default;

private:
	std::string m_reason;
};
