#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <cdt/ast.h>

namespace lx 
{
	// Forward declarations
	class Assembler;

	// Standard library functions

	namespace core
	{
		void printFunction(FunctionCall* call, Assembler& assembler);

		extern std::unordered_map <std::string, std::function<void(FunctionCall*, Assembler&)>> funcMap;
	};
}