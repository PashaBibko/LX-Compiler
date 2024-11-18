// ======================================================================================= //
//                                                                                         //
// This code is license under a Proprietary License for LX - Compiler                      //
//                                                                                         //
// Copyright(c) 2024 Pasha Bibko                                                           //
//                                                                                         //
// 1. License Grant                                                                        //
//     You are granted a non - exclusive, non - transferable, and revocable                //
//     license to use this software for personal, educational, non - commercial,           //
//     or internal commercial purposes.You may install and use the software on             //
//     your devices or within your company, but you may not sell, sublicense,              //
//     or distribute the software in any form, either directly or as part                  //
//     of any derivative works. You may privately modify the software for                  //
//     internal use within your organization, provided that the modified versions          //
//     are not distributed, shared, or otherwise made available to third parties.          //
//                                                                                         //
// 2. Freedom to Share Creations                                                           //
//     You are free to create, modify, and share works or creations made with this         //
//     software, provided that you do not redistribute the original software itself.       //
//     All creations made with this software are solely your responsibility, and           //
//     you may license or distribute them as you wish, under your own terms.               //
//                                                                                         //
// 3. Restrictions                                                                         //
//     You may not:                                                                        //
//     - Sell, rent, lease, or distribute the original software or any copies              //
//       thereof, including modified versions.                                             //
//     - Distribute the software or modified versions to any third party.                  //
//                                                                                         //
// 4. Disclaimer of Warranty                                                               //
//     This software is provided "as is", without warranty of any kind, either             //
//     express or implied, including but not limited to the warranties of merchantability, //
//     fitness for a particular purpose, or non - infringement.In no event shall the       //
//     authors or copyright holders be liable for any claim, damages, or other liability,  //
//     whether in an action of contract, tort, or otherwise, arising from, out of, or in   //
//     connection with the software or the use or other dealings in the software.          //
//                                                                                         //
// ======================================================================================= //

#include <modules/modules.h>

#include <common.h>

#include <lexer.h>

namespace LX::Lexer
{
	// Constepr functions to check if a character is a letter or number
	static bool constexpr isAlphaNumeric(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'); }

	// Constepr function to check if a character is whitespace
	static bool constexpr isWhitespace(const char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

	// Who doesn't love macros
	#define SINGLE_CASE_OP(OP, TOKEN) case OP: { t.emplace_back(FuncToken::TOKEN); break; }

	#define MULTI_CASE_OP(DEFAULT) if (auto it = opMap.find((*current)[currentIndex++ + 1]); it != opMap.end()) { t.emplace_back(it->second); } else { t.emplace_back(FuncToken::DEFAULT); } break;

	static const std::unordered_map<std::string, FuncToken::Type> keywords =
	{
		// Variables
		{ "int", FuncToken::INT_DEC },
		{ "string", FuncToken::STR_DEC },

		{ "const", FuncToken::CONST },

		// Control flow
		{ "if", FuncToken::IF },
		{ "elif", FuncToken::ELIF },
		{ "else", FuncToken::ELSE },

		{ "for", FuncToken::FOR },
		{ "while", FuncToken::WHILE },

		{ "break", FuncToken::BREAK },
		{ "continue", FuncToken::CONTINUE },
		{ "return", FuncToken::RETURN },

		// Logical
		{ "&&", FuncToken::AND },
		{ "and", FuncToken::AND },

		{ "||", FuncToken::OR },
		{ "or", FuncToken::OR },

		{ "not", FuncToken::NOT }
	};

	static void lexFunctionFromStringView(LexerStreamSect& sect, std::vector<FuncToken>& t, const std::string_view* current)
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
				else { t.emplace_back(FuncToken::IDENTIFIER, word); }
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
						t.emplace_back(FuncToken::STRING_LITERAL, std::string(current->substr(stringStart, currentIndex - stringStart)));

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
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '+', FuncToken::INCREMENT },
							{ '=', FuncToken::PLUS_EQUALS }
						};

						MULTI_CASE_OP(PLUS)
					}

					case '-':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '-', FuncToken::DECREMENT },
							{ '=', FuncToken::MINUS_EQUALS },
							{ '>', FuncToken::ARROW }
						};

						MULTI_CASE_OP(MINUS)
					}

					case '*':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '=', FuncToken::MULTIPLY_EQUALS }
						};

						MULTI_CASE_OP(MULTIPLY)
					}

					case '/':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '=', FuncToken::DIVIDE_EQUALS }
						};

						MULTI_CASE_OP(DIVIDE)
					}

					case '=':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '=', FuncToken::EQUALS },
							{ '>', FuncToken::DOUBLE_ARROW }
						};

						MULTI_CASE_OP(ASSIGN)
					}

					// These all currently only have one multichar case but this will be expanded later (bitwise operators for example)

					case '!':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '=', FuncToken::NOT_EQUALS }
						};

						MULTI_CASE_OP(NOT)
					}

					case '>':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '=', FuncToken::GREATER_THAN_EQUALS }
						};

						MULTI_CASE_OP(GREATER_THAN)
					}

					case '<':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ '=', FuncToken::LESS_THAN_EQUALS }
						};

						MULTI_CASE_OP(LESS_THAN)
					}

					case ':':
					{
						static std::unordered_map<char, FuncToken::Type> opMap
						{
							{ ':', FuncToken::DOUBLE_COLON }
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
		std::vector<FuncToken>& t = sect.creator.getFunctionTokens();

		// Lexes the block
		t.emplace_back(FuncToken::LEFT_BRACE);
		lexFunctionFromStringView(sect, t, sect.getBlock());
		t.emplace_back(FuncToken::RIGHT_BRACE);
		t.emplace_back(FuncToken::END_OF_SCOPE);
	}
}