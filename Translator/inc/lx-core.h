#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <cdt/ast.h>

namespace LX::Translator
{
	// Forward declarations
	class Assembler;

	// Standard library functions

	namespace Core
	{
		void printFunction(LX::Parser::FunctionCall* call, Assembler& assembler);

		extern std::unordered_map <std::string, std::function<void(LX::Parser::FunctionCall*, Assembler&)>> funcMap;
	};
}