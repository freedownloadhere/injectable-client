#pragma once
#include "pch.hpp"
#include "Exception.hpp"

class JavaException : public Exception {
public:
	JavaException(const std::string& reason) : Exception(reason) { }
};
