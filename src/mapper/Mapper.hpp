#pragma once
#include "pch.hpp"

class Mapper {
public:
	using MapType = std::unordered_map<std::string_view, std::string_view>;

	Mapper(MapType&& entries);

	std::string_view map(std::string_view str) const;

private:
	MapType m_entries;
};
