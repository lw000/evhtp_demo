#pragma once

#include <string>

class ConverCode
{
public:
	static std::string gbk_to_utf8(const std::string& str);
	static std::string utf8_to_gbk(const std::string& str);
};