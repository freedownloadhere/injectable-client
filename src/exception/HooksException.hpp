#pragma once
#include "pch.hpp"
#include "Exception.hpp"

class HooksException : public Exception {
public:
	HooksException(const std::string& reason) : Exception(reason) { }
};
