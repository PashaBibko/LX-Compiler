#include <lexer.h>

#include <modules/modules.h>
#include <cdt/token.h>

#include <debug/error.h>
#include <debug/Log.h>

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>

#include <fstream>
#include <sstream>

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

	LexerStreamSect::LexerStreamSect(std::string_view identifier, std::string_view block, Lexer& creator) : identifier(identifier), block(block), creator(creator)
	{
		// Gets length of the whitespace at the start of the identifier
		size_t id_start = std::find_if(identifier.begin(), identifier.end(),
			[](char c) { return !isWhitespace(c); }) - identifier.begin();

		// Gets the length of the whitespace at the start of the block
		size_t block_start = std::find_if(block.begin(), block.end(),
			[](char c) { return !isWhitespace(c); }) - block.begin();

		// Trims the identifier and block

		identifier = identifier.substr(id_start);
		block = block.substr(block_start);

		// Gets the size of the first word
		size_t wordSize = std::find_if(identifier.begin(), identifier.end(),
			[](char c) { return isWhitespace(c) || c == '['; }) - identifier.begin();

		// The first word of the block (which is the type of the block)
		// Ends when it reaches a non-alphabetical character
		std::string_view blockType = identifier.substr(0, wordSize);

		// The info inside <> is optinional but is still needed to be got before the lexer function is called
		if (identifier[blockType.size()] == '[')
		{
			// The last character of the first word
			// This is used to find the end of the <> block
			size_t endOfInfo = identifier.find_first_of(']');

			// Throws an error if the end of the <> block is not found
			if (endOfInfo == std::string::npos)
			{
				THROW_ERROR("Expected a '[' to end the '[]' block");
			}

			// Sets the info to the block
			info = identifier.substr(blockType.size() + 1, endOfInfo - blockType.size() - 1);
		}

		else
		{
			// Sets it to the default value
			info = "void";
		}

		// Sets the type of the block
		if (auto it = sectTypeMap.find(std::string(blockType)); it != sectTypeMap.end()) { type = it->second; }

		// Throws an error if the block type is not found
		else { THROW_ERROR("Unknown block type: " + std::string(blockType)); }
	}

	void LexerStreamSect::generateTokens()
	{
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
		std::cout << "Info: " << info << std::endl;
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
					// Adds EOF token so the parser knows when to stop
					funcTokens.emplace_back(TokenType::END_OF_FILE);

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
