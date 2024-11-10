#pragma once

#include <assembler.h>

#include <cdt/ast.h>

#include <sstream>

namespace lx
{
	void assembleIdentifier(Assembler& assembler, ASTNode* node);

	void assembleVariableDeclaration(Assembler& assembler, ASTNode* node);

	void assembleAssignment(Assembler& assembler, ASTNode* node);

	void assembleOperation(Assembler& assembler, ASTNode* node);

	void assembleUnaryOperation(Assembler& assembler, ASTNode* node);

	void assembleFunctionCall(Assembler& assembler, ASTNode* node);

	void assembleStringLiteral(Assembler& assembler, ASTNode* node);

	void assembleBracketedExpression(Assembler& assembler, ASTNode* node);

	void assembleIfStatement(Assembler& assembler, ASTNode* node);

	void assembleReturnStatement(Assembler& assembler, ASTNode* node);

	void assembleUndefined(Assembler& assembler, ASTNode* node);
}