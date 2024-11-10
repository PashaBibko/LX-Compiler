#pragma once

#include <cdt/ast.h>

#include <debug/Log.h>

namespace LX::Parser
{
	class Parser
	{
	private:
		// Current tokens
		const std::vector<LX::Lexer::Token>* currentTokens;

		// Current index in the tokens
		size_t currentIndex = 0;

		std::vector<std::unique_ptr<ASTNode>> parseBlock();

		std::unique_ptr<ASTNode> parsePrimary();

		std::unique_ptr<ASTNode> parseUnaryOperation();
		std::unique_ptr<ASTNode> parseOperation();

		std::unique_ptr<ASTNode> parseFunctionCall();

		std::unique_ptr<ASTNode> parseAssignment();

		std::unique_ptr<ASTNode> parseReturnStatement();

		std::unique_ptr<ASTNode> parseVariableDeclaration();

		std::unique_ptr<ASTNode> parseIfStatement();

		FunctionDeclaration parseFunctionDeclaration();

	public:
		Parser() {}

		void parse(const std::vector<LX::Lexer::Token>& tokens, FileAST& out);
	};
}