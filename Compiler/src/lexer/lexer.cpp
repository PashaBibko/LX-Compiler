#include <lexer/lexer.h>

#include <lexer/token.h>

#include <stdexcept>
#include <iostream>

#include <Util/trans-table.h>

static constexpr bool isAlpha(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static constexpr bool isWhitespace(const char c) { return (c == ' ' || c == '\t') || (c == '\n' || c == '\r'); }
static constexpr bool isEndOfComment(const char c) { return (c == '\n' || c == '#'); }
static constexpr bool isNumeric(const char c) { return (c >= '0' && c <= '9'); }

static constexpr bool isAlphaNumeric(const char c) { return isAlpha(c) || isNumeric(c); }

namespace lx
{
	std::string Lexer::lexString()
	{
		// The start index of the string
		size_t stringStartIndex = ++currentIndex;

		// Loops through the source code until the end of the string is reached
		while (currentIndex < currentSource->length() && currentSource->operator[](currentIndex) != '"')
			currentIndex++;

		// Returns the output token
		return currentSource->substr(stringStartIndex, currentIndex - stringStartIndex);
	}

	TokenType Lexer::lexPlusOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: ++, +=
		switch (nextChar)
		{
		case '+':
			currentIndex++;
			return TokenType::INCREMENT;

		case '=':
			currentIndex++;
			return TokenType::PLUS_EQUALS;

		default:
			return TokenType::PLUS;
		}
	}

	TokenType Lexer::lexMinusOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: --, -=, ->
		switch (nextChar)
		{
		case '-':
			currentIndex++;
			return TokenType::DECREMENT;

		case '=':
			currentIndex++;
			return TokenType::MINUS_EQUALS;

		case '>':
			currentIndex++;
			return TokenType::ARROW;

		default:
			return TokenType::MINUS;
		}
	}

	TokenType Lexer::lexMultiplyOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: *= **
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return TokenType::MULTIPLY_EQUALS;

		case '*':
			currentIndex++;
			return TokenType::POWER;

		default:
			return TokenType::MULTIPLY;
		}
	}

	TokenType Lexer::lexDivideOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: /=, //
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return TokenType::DIVIDE_EQUALS;

		default:
			return TokenType::DIVIDE;
		}
	}

	TokenType Lexer::lexEqualsOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: ==, =>
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return TokenType::EQUALS;

		case '>':
			currentIndex++;
			return TokenType::DOUBLE_ARROW;

		default:
			return TokenType::ASSIGN;
		}
	}

	TokenType Lexer::lexNotOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: !=
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return TokenType::NOT_EQUALS;

		default:
			return TokenType::NOT;
		}
	}

	TokenType Lexer::lexLessThanOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: <=
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return TokenType::LESS_THAN_EQUALS;

		default:
			return TokenType::LESS_THAN;
		}
	}

	TokenType Lexer::lexGreaterThanOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: >=
		switch (nextChar)
		{
		case '=':
			currentIndex++;
			return TokenType::GREATER_THAN_EQUALS;

		default:
			return TokenType::GREATER_THAN;
		}
	}

	TokenType Lexer::lexColonOperator()
	{
		const char nextChar = currentSource->operator[](currentIndex + 1);

		// Possible operators: ::
		switch (nextChar)
		{
		case ':':
			currentIndex++;
			return TokenType::DOUBLE_COLON;

		default:
			return TokenType::COLON;
		}
	}

	Token Lexer::lexMultiChar()
	{
		// The length of the word
		size_t wordStartIndex = currentIndex;

		// Loops through the source code until the whitespace is reached
		while (currentIndex < currentSource->length() && isAlphaNumeric(currentSource->operator[](currentIndex)))
			currentIndex++;

		// Turns the word into a string
		const std::string word = currentSource->substr(wordStartIndex, currentIndex - wordStartIndex);
		currentIndex--;

		// Checks if the word is a keyword
		if (keywords.find(word) == keywords.end())
		{
			return Token(TokenType::IDENTIFIER, word);
		}

		// Else, returns the identifier token
		else
		{
			return Token(keywords.at(word), "");
		}
	}

	std::vector<Token> Lexer::lex(const std::string& input)
	{
		std::vector<Token> tokens;

		// The current source code (passed by reference for performance)
		currentSource = &input;

		// Sets lexer variables to their default values
		currentIndex = 0;

		const size_t sourceLength = currentSource->length();

		// Loops through the source code until the end is reached
		while (currentIndex < sourceLength)
		{
			//  Gets the current character
			const char currentChar = currentSource->operator[](currentIndex);

			// Checks if the next character is whitespace
			bool isNextCharWhitespace = false;

			size_t nextIndex = currentIndex + 1;

			if (nextIndex < sourceLength)
				isNextCharWhitespace = isWhitespace(currentSource->operator[](nextIndex));

			// Switch statement for the current character
			switch (currentChar)
			{
				// Skips whitespace
				case '\n':
				case ' ':
				case '\t':
				case '\r':
					break;

				// Skips comments
				case '#':
					do { currentIndex++; } while (currentIndex < sourceLength && !isEndOfComment(currentSource->operator[](currentIndex)));
					break;

				// Single character tokens
				// These tokens do not matter if the next character is not whitespace
				case ';':
					tokens.emplace_back(TokenType::SEMICOLON);
					break;

				case ',':
					tokens.emplace_back(TokenType::COMMA);
					break;

				case '.':
					tokens.emplace_back(TokenType::DOT);
					break;

				case '%':
					tokens.emplace_back(TokenType::MODULO);
					break;

				// Brackets
				// Why are there three different types of brackets?
				case '(':
					tokens.emplace_back(TokenType::LEFT_PAREN);
					break;

				case ')':
					tokens.emplace_back(TokenType::RIGHT_PAREN);
					break;

				case '{':
					tokens.emplace_back(TokenType::LEFT_BRACE);
					break;

				case '}':
					tokens.emplace_back(TokenType::RIGHT_BRACE);
					break;

				case '[':
					tokens.emplace_back(TokenType::LEFT_BRACKET);
					break;

				case ']':
					tokens.emplace_back(TokenType::RIGHT_BRACKET);
					break;

				// Single character tokens that can be part of a multicharacter token
				case '+': tokens.emplace_back(lexPlusOperator()); break;
				case '-': tokens.emplace_back(lexMinusOperator()); break;
				case '*': tokens.emplace_back(lexMultiplyOperator()); break;
				case '/': tokens.emplace_back(lexDivideOperator()); break;
				case '=': tokens.emplace_back(lexEqualsOperator()); break;
				case '!': tokens.emplace_back(lexNotOperator()); break;
				case '<': tokens.emplace_back(lexLessThanOperator()); break;
				case '>': tokens.emplace_back(lexGreaterThanOperator()); break;
				case ':': tokens.emplace_back(lexColonOperator()); break;

				// String literals
				case '"':
					tokens.emplace_back(TokenType::STRING_LITERAL, lexString());
					break;

				// Multicharacter tokens
				default:
					if (isAlphaNumeric(currentChar))
					{
						tokens.push_back(lexMultiChar());
						break;
					}

					else
					{
						std::cerr << "Unknown character: " << currentChar << std::endl;
						return std::vector<Token>();
					}
			}


			// Increments the current index
			currentIndex++;
		}

		// Adds EOF token
		tokens.emplace_back(TokenType::END_OF_FILE, "");

		// Returns the tokens
		return tokens;
	}
}
