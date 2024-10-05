#pragma once

#include <lexer/token.h>

#include <stdexcept>

#include <Util/trans-table.h>
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
		Token lexString();

		// Seperate functions for each operator for higher efficiency

		Token lexPlusOperator();
		Token lexMinusOperator();
		Token lexMultiplyOperator();
		Token lexDivideOperator();

		Token lexEqaulsOperator();
		Token lexNotOperator();

		Token lexLessThanOperator();
		Token lexGreaterThanOperator();

		Token lexColonOperator();

		// Multi-character operators

		Token lexMultiChar();

	public:
		// Translation table for keywords
		static const TransTable<std::string, TokenType> keywords;

		Lexer() : currentSource(nullptr) {}

		std::vector<Token> lex(const std::string& input);
	};

	inline Token Lexer::lexPlusOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: ++, +=
		switch (nextChar)
		{
		case '+':
			currentIndex++;
			return Token(TokenType::INCREMENT);

		case '=':
			currentIndex++;
			return Token(TokenType::PLUS_EQUALS);

		default:
			return Token(TokenType::PLUS);
		}
	}

	inline Token Lexer::lexMinusOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: --, -=, ->
		switch (nextChar)
		{
		case '-':
			currentIndex++;
			return Token(TokenType::DECREMENT);

		case '=':
			currentIndex++;
			return Token(TokenType::MINUS_EQUALS);

		case '>':
			currentIndex++;
			return Token(TokenType::ARROW);

		default:
			return Token(TokenType::MINUS);
		}
	}

	inline Token Lexer::lexMultiplyOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: *= **
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return Token(TokenType::MULTIPLY_EQUALS);

		case '*':
			currentIndex++;
			return Token(TokenType::POWER);

		default:
			return Token(TokenType::MULTIPLY);
		}
	}

	inline Token Lexer::lexDivideOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: /=, //
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return Token(TokenType::DIVIDE_EQUALS);

		default:
			return Token(TokenType::DIVIDE);
		}
	}

	inline Token Lexer::lexEqaulsOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: ==, =>
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return Token(TokenType::EQUALS);

		case '>':
			currentIndex++;
			return Token(TokenType::DOUBLE_ARROW);

		default:
			return Token(TokenType::ASSIGN);
		}
	}

	inline Token Lexer::lexNotOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: !=
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return Token(TokenType::NOT_EQUALS);

		default:
			return Token(TokenType::NOT);
		}
	}

	inline Token Lexer::lexLessThanOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: <=
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return Token(TokenType::LESS_THAN_EQUALS);

		default:
			return Token(TokenType::LESS_THAN);
		}
	}

	inline Token Lexer::lexGreaterThanOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: >=
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return Token(TokenType::GREATER_THAN_EQUALS);

		default:
			return Token(TokenType::GREATER_THAN);
		}
	}

	inline Token Lexer::lexColonOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: ::
		switch (nextChar)
		{
		case ':':
			currentIndex++;
			return Token(TokenType::DOUBLE_COLON);

		default:
			return Token(TokenType::COLON);
		}
	}
}