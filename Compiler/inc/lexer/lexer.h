#pragma once

#include <lexer/token.h>

#include <unordered_map>
#include <stdexcept>

namespace lx
{
	class Lexer
	{
		private:
			// Current source code being lexed
			const std::string* currentSource;

			// Current index in the source code
			size_t currentIndex = 0;

			// Function for string literals
			std::string lexString();

			// Seperate functions for each operator for higher efficiency

			TokenType lexPlusOperator();
			TokenType lexMinusOperator();
			TokenType lexMultiplyOperator();
			TokenType lexDivideOperator();

			TokenType lexEqualsOperator();
			TokenType lexNotOperator();

			TokenType lexLessThanOperator();
			TokenType lexGreaterThanOperator();

			TokenType lexColonOperator();

			// Multi-character operators

			Token lexMultiChar();

		public:
			// Translation table for keywords
			static const std::unordered_map<std::string, TokenType> keywords;

			Lexer() : currentSource(nullptr) {}

			const std::vector<Token> lex(const std::string& input);
	};
}