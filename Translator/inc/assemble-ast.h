#pragma once

#include <assembler.h>

#include <cdt/ast.h>

#include <sstream>

namespace LX::Translator
{
	void assembleIdentifier(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleVariableDeclaration(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleAssignment(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleOperation(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleUnaryOperation(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleFunctionCall(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleStringLiteral(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleBracketedExpression(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleIfStatement(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleReturnStatement(Assembler& assembler, LX::Parser::ASTNode* node);

	void assembleUndefined(Assembler& assembler, LX::Parser::ASTNode* node);
}