#pragma once

#include <translator.h>

#include <cdt/ast.h>

#include <sstream>

namespace LX::Translator
{
	void assembleIdentifier(Translator& translator, LX::Parser::ASTNode* node);

	void assembleVariableDeclaration(Translator& translator, LX::Parser::ASTNode* node);

	void assembleAssignment(Translator& translator, LX::Parser::ASTNode* node);

	void assembleOperation(Translator& translator, LX::Parser::ASTNode* node);

	void assembleUnaryOperation(Translator& translator, LX::Parser::ASTNode* node);

	void assembleFunctionCall(Translator& translator, LX::Parser::ASTNode* node);

	void assembleStringLiteral(Translator& translator, LX::Parser::ASTNode* node);

	void assembleBracketedExpression(Translator& translator, LX::Parser::ASTNode* node);

	void assembleIfStatement(Translator& translator, LX::Parser::ASTNode* node);

	void assembleReturnStatement(Translator& translator, LX::Parser::ASTNode* node);

	void assembleUndefined(Translator& translator, LX::Parser::ASTNode* node);
}