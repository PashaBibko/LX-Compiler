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

#include <lexer.h>

#include <common.h>

#include <modules/modules.h>

static constexpr bool isAlpha(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static constexpr bool isWhitespace(const char c) { return (c == ' ' || c == '\t') || (c == '\n' || c == '\r'); }

namespace LX::Lexer
{
	const std::unordered_map<std::string, SectType> LexerStreamSect::sectTypeMap =
	{
		{ "func", SectType::FUNCTION },
		{ "shader", SectType::SHADER },
		{ "struct", SectType::STRUCT },
		{ "alias", SectType::MACRO },
		{ "class", SectType::CLASS },
		{ "enum", SectType::ENUM }
	};

	const std::unordered_map<SectType, std::function<void(LexerStreamSect&)>> LexerStreamSect::modules =
	{
		{ SectType::FUNCTION, lexFunction },
		{ SectType::SHADER, lexShader },
		{ SectType::STRUCT, lexClass },
		{ SectType::MACRO, lexMacro },
		{ SectType::CLASS, lexClass },
		{ SectType::ENUM, lexEnum }
	};

	LexerStreamSect::LexerStreamSect(std::string_view identifier, std::string_view block, Lexer& creator) : creator(creator)
	{
		// Gets length of the whitespace at the start of the identifier
		size_t id_start = std::find_if(identifier.begin(), identifier.end(),
			[](char c) { return !isWhitespace(c); }) - identifier.begin();

		// Gets the length of the whitespace at the start of the block
		size_t block_start = std::find_if(block.begin(), block.end(),
			[](char c) { return !isWhitespace(c); }) - block.begin();

		// Trims the identifier and block
		this->identifier = identifier.substr(id_start);
		this->block = block.substr(block_start);
	}

	void LexerStreamSect::lexIdentifier()
	{
		THROW_ERROR("Pasha couldnt be asked to code this on a monday morning");
	}

	void LexerStreamSect::generateTokens()
	{
		// Lexes the identifier
		lexIdentifier();

		// Tries to find the module for the block type
		if (auto it = modules.find(type); it != modules.end()) { it->second(*this); }

		// If none is found throws an error
		else { THROW_ERROR("No lexer module found for block type: " + std::to_string(static_cast<int>(type))); }
	}

	void LexerStreamSect::debugDisplay() const
	{
		// Fancy debug display
		std::cout << "=============================================================== " << std::endl;
		std::cout << "LexerStreamSect Debug Display\n" << std::endl;
		std::cout << "Type: " << static_cast<int>(type) << std::endl;
		std::cout << "----------------------------- Identifier ---------------------- " << std::endl;
		std::cout << identifier << std::endl;
		std::cout << "----------------------------- Block --------------------------- " << std::endl;
		std::cout << block << std::endl;
		std::cout << "=============================================================== " << std::endl;

	}

	Lexer::Lexer(const std::string& source, const bool debug) : currentSource(source), debug(debug)
	{
		// Checks that the source is not empty
		if (source.empty())
		{
			THROW_ERROR("Lexer must be initialized with a source code string");
		}

		// If the source is not empty, then it will create the blocks
		// The creation of the blocks will also turn them into tokens and add them to the the respective vectors

		// How many braces deep we are
		// Only the top level blocks are considered when added to the full stream
		// They are things like different types of functions, structs, etc.
		// This will be need to be called again for classes becuase they can have nested blocks
		int depth = 0;

		// Variables to keep track of the std::string_view size
		size_t blockStart = 0;
		size_t endOfLastBlock = 0;

		// Variable to keep track of the index of the string
		// Only used by the lexer itself
		size_t currentIndex = 0;

		// Variables to keep track of the current line and column
		// These are not used by the lexer itself and are only to make debugging easier for the user
		// Will be passed onto the LexerStreamSect to help with that section of debugging (ADD LATER)
		// Not actually used at all within this function yet
		size_t currentLine = 1;
		size_t currentColumn = 0;

		// Block loop
		// It is escaped via return
		while (true)
		{
			// The current character
			// Ive been told to have less useless comments but green looks cool
			// Gyula hates it when I do this
			// I'm sorry Gyula (not)
			char currentChar = currentSource[currentIndex];

			switch (currentChar)
			{
				// Whitespace cases
				case ' ':
				case '\t':
				{
					// These cases mean nothing to the block lexer
					break;
				}

				case '\n':
				{
					// Resets the column and increments the line
					currentLine++;
					currentColumn = 0;

					break;
				}

				case '\0':
				{
					// Breaks out of the loop
					return;
				}

				// Parses block section
				case '{':
				{
					// If the depth is 0, then we are at the top level block
					// Else we are in a nested block so it does nothing to blockStart
					blockStart = ((depth == 0) ? currentIndex + 1 : blockStart);
					depth++;

					break;
				}

				case '}':
				{
					// If the depth is 1, then we are at the end of the top level block
					if (depth == 1)
					{
						// Creates the block
						LexerStreamSect s
						(
							std::string_view(currentSource.data() + endOfLastBlock, blockStart - endOfLastBlock - 1),
							std::string_view(currentSource.data() + blockStart, currentIndex - blockStart),
							*this
						);

						// Generates the tokens (automatically adds them to the relevant vector)
						s.generateTokens();
						
						// If debug is enabled, then it will add the block to the debug vector
						if (debug) { sections.push_back(s); }

						endOfLastBlock = currentIndex + 1;
					}

					depth--;

					break;
				}

				// Default case
				default:
				{
					break;
				}
			}

			// Iterates the index and column
			currentColumn++;
			currentIndex++;
		}
	}
}
