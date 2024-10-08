#pragma once

#include <assembler/lx-core.h>
#include <parser/parser.h>

namespace lx 
{
	class Assembler
	{
		private:
			std::unordered_map<std::string, bool> includes;

			std::string assembleFunctionCall(FunctionCall* call);

			std::string assembleIdentifier(Identifier* id);

			std::string assembleStringLiteral(StringLiteral* str);

			std::string assembleAssignment(Assignment* assign);

			std::string assembleVarDecType(Identifier* t);
			std::string assembleVarMods(VariableDeclaration* var);
			std::string assembleVarDec(VariableDeclaration* var);

			std::string assembleOperand(TokenType op);
			std::string assembleOperation(Operation* op);

			std::string assembleUnaryOperation(UnaryOperation* op);

			std::string assembleIfStatement(IfStatement* ifStmt);

			std::string assembleBracketExpression(BracketedExpression* bracket);

			std::string assembleReturnStatement(ReturnStatement* ret);

		public:
			std::string assembleNode(std::unique_ptr<ASTNode>& node);

			Assembler() = default;

			std::string assemble(FileAST& AST);
	};
}
