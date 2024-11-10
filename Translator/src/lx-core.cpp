#include <lx-core.h>

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <cdt/ast.h>

#include <translator.h>

namespace LX::Translator
{
	void Core::printFunction(LX::Parser::FunctionCall* call, Translator& translator)
	{
		translator.includes.insert("iostream");
		translator.out << "std::cout";

		for (std::unique_ptr<LX::Parser::ASTNode>& arg : call->args)
		{
			translator.out << " << ";
			translator.assembleNode(arg.get());
		}

		translator.out << " << std::endl;\n";
	}

	// Core function map

	std::unordered_map <std::string, std::function<void(LX::Parser::FunctionCall*, Translator&)>> Core::funcMap =
	{
		{"print", printFunction}
	};
}
