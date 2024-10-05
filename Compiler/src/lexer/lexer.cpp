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
	Token Lexer::lexString()
	{
		// Creates the output token
		Token out = { TokenType::STRING_LITERAL, "" };

		// The length of the string
		size_t lengthOfString = 1;

		// Increments to avoid the first quote
		currentIndex++;

		// Loops through the source code until the end of the string is reached
		while (currentIndex + lengthOfString < currentSource->length() && currentSource->operator[](currentIndex + lengthOfString) != '"')
			lengthOfString++;

		// Turns the string into a string
		out.value = currentSource->substr(currentIndex, lengthOfString);

		// Sets the current index to the end of the string
		currentIndex = currentIndex + lengthOfString;

		// Returns the output token
		return out;
	}

	Token Lexer::lexMultiChar()
	{
		// Creates the output token
		Token out;

		// The length of the word
		size_t lengthOfWord = 1;

		// Loops through the source code until the whitespace is reached
		while (currentIndex + lengthOfWord < currentSource->length() && isAlphaNumeric(currentSource->operator[](currentIndex + lengthOfWord)))
			lengthOfWord++;

		// Turns the word into a string
		const std::string word = currentSource->substr(currentIndex, lengthOfWord);

		TokenType tokenType = TokenType::UNDEFINED;

		try
		{
			tokenType = keywords[word];
		}

		catch (std::out_of_range)
		{
			tokenType = TokenType::IDENTIFIER;
			out.value = word;
		}

		out.type = tokenType;

		currentIndex = currentIndex + lengthOfWord - 1;

		return out;
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
				break;

			case ' ':
				break;

			case '\t':
				break;


			case '\r':
				break;

				// Skips comments
			case '#':
				do { currentIndex++; } while (currentIndex < sourceLength && !isEndOfComment(currentSource->operator[](currentIndex)));
				break;

				// Single character tokens
				// These tokens do not matter if the next character is not whitespace
			case ';':
				tokens.push_back(Token(TokenType::SEMICOLON));
				break;

			case ',':
				tokens.push_back(Token(TokenType::COMMA));
				break;

			case '.':
				tokens.push_back(Token(TokenType::DOT));
				break;

			case '%':
				tokens.push_back(Token(TokenType::MODULO));
				break;

				// Brackets
				// Why are there three different types of brackets?
			case '(':
				tokens.push_back(Token(TokenType::LEFT_PAREN));
				break;

			case ')':
				tokens.push_back(Token(TokenType::RIGHT_PAREN));
				break;

			case '{':
				tokens.push_back(Token(TokenType::LEFT_BRACE));
				break;

			case '}':
				tokens.push_back(Token(TokenType::RIGHT_BRACE));
				break;

			case '[':
				tokens.push_back(Token(TokenType::LEFT_BRACKET));
				break;

			case ']':
				tokens.push_back(Token(TokenType::RIGHT_BRACKET));
				break;

				// Single character tokens that can be part of a multicharacter token
			case '+':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::PLUS) : lexPlusOperator());
				break;

			case '-':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::MINUS) : lexMinusOperator());
				break;

			case '*':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::MULTIPLY) : lexMultiplyOperator());
				break;

			case '/':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::DIVIDE) : lexDivideOperator());
				break;

			case '=':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::ASSIGN) : lexEqaulsOperator());
				break;

			case '!':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::NOT) : lexNotOperator());
				break;

			case '<':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::LESS_THAN) : lexLessThanOperator());
				break;

			case '>':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::GREATER_THAN) : lexGreaterThanOperator());
				break;

			case ':':
				tokens.push_back(isNextCharWhitespace ? Token(TokenType::COLON) : lexColonOperator());
				break;

				// String literals
			case '"':
				tokens.push_back(lexString());
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
		tokens.push_back(Token(TokenType::END_OF_FILE));

		// Returns the tokens
		return tokens;
	}
}