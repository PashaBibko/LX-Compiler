#include <modules/modules.h>

#include <debug/error.h>

namespace lx
{
	// Constepr functions to check if a character is a letter or number
	static bool constexpr isAlphaNumeric(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'); }

	// Constepr function to check if a character is whitespace
	static bool constexpr isWhitespace(const char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

	// Who doesn't love macros
	#define SINGLE_CASE_OP(OP, TOKEN) case OP: { t.emplace_back(TokenType::TOKEN); break; }

	#define MULTI_CASE_OP(DEFAULT) if (auto it = opMap.find((*current)[currentIndex++ + 1]); it != opMap.end()) { t.emplace_back(it->second); } else { t.emplace_back(TokenType::DEFAULT); } break;

	static const std::unordered_map<std::string, TokenType> keywords =
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

	static void lexFunctionFromStringView(LexerStreamSect& sect, std::vector<Token>& t, const std::string_view* current)
	{
		// Variables to keep track of the current index and the length of the current string
		size_t currentIndex = 0;
		size_t currentLength = current->size();

		while (currentIndex < currentLength)
		{
			// Skips whitespace
			if (isWhitespace((*current)[currentIndex]));

			// Handles multicharacter words / numbers
			else if (isAlphaNumeric((*current)[currentIndex]))
			{
				// Gets the start of the word
				size_t wordStart = currentIndex;

				// Loops until it reaches a non-alphanumeric character
				while (currentIndex < currentLength && isAlphaNumeric((*current)[currentIndex++]));

				// Decrements (for some reason)
				currentIndex--;

				// Stores the word in a string
				const std::string word = std::string(current->substr(wordStart, currentIndex - wordStart));

				// Decrements again (for some reason)
				currentIndex--;

				// Finds wether the token is a keyword or an identifier
				if (auto it = keywords.find(word); it != keywords.end()) { t.emplace_back(it->second); }
				else { t.emplace_back(TokenType::IDENTIFIER, word); }
			}

			// Else it will be an operator or an invalid character
			else
			{
				// This switch case goes through all the possible operators
				// If it is not an operator then it will throw an error
				switch ((*current)[currentIndex])
				{
					// Special operators
					case '"':
					{
						// Gets the start of the string
						size_t stringStart = ++currentIndex;

						// Loops until it reaches the closing quote
						// \" will currently end it. I should fix this later
						// ^ Written on 04/11/24. Please dont be a year later when I come back to this
						while (currentIndex < currentLength && (*current)[currentIndex++] != '"');

						// Decrements to avoid skipping the next character
						currentIndex--;

						// Uses emplace instead of push to avoid copying
						// Unsure if std::string_view would be more efficient as this has a higher cache hit rate
						t.emplace_back(TokenType::STRING_LITERAL, std::string(current->substr(stringStart, currentIndex - stringStart)));

						break;
					}

					// Single case operators (Macro defined near the top of this file)
					SINGLE_CASE_OP(';', SEMICOLON)
					SINGLE_CASE_OP(',', COMMA)
					SINGLE_CASE_OP('.', DOT)
					SINGLE_CASE_OP('%', MODULO)

					SINGLE_CASE_OP('(', LEFT_PAREN)
					SINGLE_CASE_OP(')', RIGHT_PAREN)

					SINGLE_CASE_OP('{', LEFT_BRACE)
					SINGLE_CASE_OP('}', RIGHT_BRACE)

					SINGLE_CASE_OP('[', LEFT_BRACKET)
					SINGLE_CASE_OP(']', RIGHT_BRACKET)

					// Multi-case operators (Macro defined near the top of this file)
					case '+':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '+', TokenType::INCREMENT },
							{ '=', TokenType::PLUS_EQUALS }
						};

						MULTI_CASE_OP(PLUS)
					}

					case '-':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '-', TokenType::DECREMENT },
							{ '=', TokenType::MINUS_EQUALS },
							{ '>', TokenType::ARROW }
						};

						MULTI_CASE_OP(MINUS)
					}

					case '*':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '=', TokenType::MULTIPLY_EQUALS }
						};

						MULTI_CASE_OP(MULTIPLY)
					}

					case '/':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '=', TokenType::DIVIDE_EQUALS }
						};

						MULTI_CASE_OP(DIVIDE)
					}

					case '=':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '=', TokenType::EQUALS },
							{ '>', TokenType::DOUBLE_ARROW }
						};

						MULTI_CASE_OP(ASSIGN)
					}

					// These all currently only have one multichar case but this will be expanded later (bitwise operators for example)

					case '!':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '=', TokenType::NOT_EQUALS }
						};

						MULTI_CASE_OP(NOT)
					}

					case '>':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '=', TokenType::GREATER_THAN_EQUALS }
						};

						MULTI_CASE_OP(GREATER_THAN)
					}

					case '<':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ '=', TokenType::LESS_THAN_EQUALS }
						};

						MULTI_CASE_OP(LESS_THAN)
					}

					case ':':
					{
						static std::unordered_map<char, TokenType> opMap
						{
							{ ':', TokenType::DOUBLE_COLON }
						};

						MULTI_CASE_OP(COLON)
					}

					// Default case (ERROR)
					default:
					{
						THROW_ERROR(std::string("Invalid character in the lexer" + (*current)[currentIndex]));
					}
				}
			}

			currentIndex++;
		}
	}

	void lexFunction(LexerStreamSect& sect)
	{
		// Gets a reference to the correct vector in the lexer
		// We use this instead of via the sect because it has less overhead
		std::vector<Token>& t = sect.creator.getFunctionTokens();

		// Lexes the identifier
		lexFunctionFromStringView(sect, t, sect.getIdentifier());
		t.emplace_back(TokenType::LEFT_BRACE);

		// Lexes the block
		lexFunctionFromStringView(sect, t, sect.getBlock());
		t.emplace_back(TokenType::RIGHT_BRACE);
	}
}