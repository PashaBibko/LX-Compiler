#include <assembler/lx-core.h>

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <parser/ast.h>

#include <assembler/assembler.h>
namespace lx 
{
	void core::printFunction(FunctionCall* call, Assembler& assembler)
	{
		assembler.includes.insert("iostream");
		assembler.out << "std::cout";

		for (std::unique_ptr<ASTNode>& arg : call->args)
		{
			assembler.out << " << ";
			assembler.assembleNode(arg.get());
		}

		assembler.out << " << std::endl;\n";
	}

	// Core function map

	std::unordered_map <std::string, std::function<void(FunctionCall*, Assembler&)>> core::funcMap =
	{
		{"print", printFunction}
	};
}
