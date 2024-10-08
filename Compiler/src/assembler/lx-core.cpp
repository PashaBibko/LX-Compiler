#include <assembler/lx-core.h>

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

#include <parser/ast.h>

#include <assembler/assembler.h>
namespace lx 
{
	std::string core::printFunction(FunctionCall* call, Assembler& assembler)
	{
		std::string output = "std::cout";

		for (std::unique_ptr<ASTNode>& arg : call->args)
			output = output + " << " + assembler.assembleNode(arg);

		return output + ";";
	}

	// Core function map

	std::unordered_map <std::string, std::function<std::string(FunctionCall*, Assembler&)>> core::funcMap =
	{
		{"print", printFunction}
	};
}
