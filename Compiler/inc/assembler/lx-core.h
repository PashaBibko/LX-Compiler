#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <parser/ast.h>

// Forward declarations

class Assembler;

std::string assembleNode(std::unique_ptr<ASTNode>& node, Assembler& assembler);

// Standard library functions

namespace LXCore
{
	std::string printFunction(FunctionCall* call, Assembler& assembler)
	{
		std::string output = "std::cout";

		for (std::unique_ptr<ASTNode>& arg : call->args)
			output = output + " << " + assembleNode(arg, assembler);

		return output + ";";
	}

	std::unordered_map <std::string, std::function<std::string(FunctionCall*, Assembler&)>> funcMap =
	{
		{ "print", printFunction }
	};
};