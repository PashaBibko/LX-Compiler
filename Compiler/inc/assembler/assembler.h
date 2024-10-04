#pragma once

#include <assembler/lx-core.h>
#include <parser/parser.h>

class Assembler
{
	private:
		std::string assembleFunctionCall(FunctionCall* call);

		std::string assembleIdentifier(Identifier* id);

		std::string assembleStringLiteral(StringLiteral* str);

		std::string assembleAssignment(Assignment* assign);

		std::string assembleVarDec(VariableDeclaration* var);

	public:
		std::string assembleNode(std::unique_ptr<ASTNode>& node);

		Assembler() = default;

		std::string assemble(FileAST& AST);
};
