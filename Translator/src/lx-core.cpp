#include <lx-core.h>

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <cdt/ast.h>

#include <assembler.h>

namespace LX::Translator
{
	void Core::printFunction(LX::Parser::FunctionCall* call, Assembler& assembler)
	{
		assembler.includes.insert("iostream");
		assembler.out << "std::cout";

		for (std::unique_ptr<LX::Parser::ASTNode>& arg : call->args)
		{
			assembler.out << " << ";
			assembler.assembleNode(arg.get());
		}

		assembler.out << " << std::endl;\n";
	}

	// Core function map

	std::unordered_map <std::string, std::function<void(LX::Parser::FunctionCall*, Assembler&)>> Core::funcMap =
	{
		{"print", printFunction}
	};
}
