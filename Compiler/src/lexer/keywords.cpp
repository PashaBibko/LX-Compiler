#include <lexer/lexer.h>

#include <lexer/token.h>

#include <Util/trans-table.h>

namespace lx
{
	const std::unordered_map<std::string, TokenType> Lexer::keywords =
	{
		// Variables
		{ "int", TokenType::INT_DEC },
		{ "string", TokenType::STR_DEC },

		{ "const", TokenType::CONST },

		// Control flow
		{ "if", TokenType::IF },
		{ "elif", TokenType::ELIF },
		{ "else", TokenType::ELSE },

		{ "for", TokenType::FOR },
		{ "while", TokenType::WHILE },

		{ "break", TokenType::BREAK },
		{ "continue", TokenType::CONTINUE },
		{ "return", TokenType::RETURN },

		{ "func", TokenType::FUNCTION },
		{ "proc", TokenType::PROCEDURE },

		// Logical
		{ "&&", TokenType::AND },
		{ "and", TokenType::AND },

		{ "||", TokenType::OR },
		{ "or", TokenType::OR },

		{ "not", TokenType::NOT }
	};
}
