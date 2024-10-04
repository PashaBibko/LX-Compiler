#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <parser/ast.h>

// Forward declarations

class Assembler;

// Standard library functions

namespace LXCore
{
	std::string printFunction(FunctionCall* call, Assembler& assembler);

	extern std::unordered_map <std::string, std::function<std::string(FunctionCall*, Assembler&)>> funcMap;
};