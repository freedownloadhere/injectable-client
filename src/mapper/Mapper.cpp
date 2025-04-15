#include "pch.hpp"
#include "Mapper.hpp"

Mapper::Mapper(MapType&& entries) 
	: m_entries(std::move(entries)) { }

std::string_view Mapper::map(std::string_view str) const {
	if (m_entries.count(str) == 0)
		throw std::runtime_error("Failed to map the string " + std::string(str));

	return m_entries.at(str);
}
