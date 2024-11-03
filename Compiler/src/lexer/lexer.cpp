#include <lexer/lexer.h>

#include <lexer/token.h>

#include <algorithm>
#include <stdexcept>
#include <iostream>

static constexpr bool isAlpha(const char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static constexpr bool isWhitespace(const char c) { return (c == ' ' || c == '\t') || (c == '\n' || c == '\r'); }
static constexpr bool isEndOfComment(const char c) { return (c == '\n' || c == '#'); }
static constexpr bool isNumeric(const char c) { return (c >= '0' && c <= '9'); }

static constexpr bool isAlphaNumeric(const char c) { return isAlpha(c) || isNumeric(c); }

namespace lx
{
	std::unordered_map<std::string, SectType> LexerStreamSect::sectTypeMap =
	{
		{ "func", SectType::FUNCTION },
		{ "alias", SectType::MACRO }
	};

	LexerStreamSect::LexerStreamSect(std::string_view identifier, std::string_view block) : identifier(identifier), block(block)
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
			[](char c) { return !isAlpha(c); }) - identifier.begin();

		// The first word of the block
		// Ends when it reaches a non-alphabetical character
		std::string_view blockType = identifier.substr(0, wordSize);

		// The info inside the <> block
		// If none is found it is set to "void"
		std::string info;

		// The info inside <> is optinional but is still needed to be got before the lexer function is called
		if (identifier[blockType.size()] == '[')
		{
			// The last character of the first word
			// This is used to find the end of the <> block
			size_t endOfInfo = identifier.find_first_of(']');

			// Throws an error if the end of the <> block is not found
			if (endOfInfo == std::string::npos)
			{
				THROW_ERROR("Expected '[' to end the '[]' block");
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
		auto it = sectTypeMap.find(std::string(blockType));

		if (it != sectTypeMap.end())
		{
			type = it->second;
		}

		else
		{
			THROW_ERROR("Unknown block type: " + std::string(blockType));
		}
	}

	// Function to create the blocks for the lexer to handle
	void Lexer::createBlocks()
	{
		// How many braces deep we are
		// Only the top level blocks are considered when added to the full stream
		// They are things like different types of functions, structs, etc.
		// This will be need to be called again for classes becuase they can have nested blocks q	
		int depth = 0;

		// Variables to keep track of the std::string_view size
		size_t blockStart = 0;
		size_t endOfLastBlock = 0;

		// Variable to keep track of the index of the string
		// Only used by the lexer itself
		size_t currentIndex = 0;

		// Variables to keep track of the current line and column
		// These are not used by the lexer itself and are only to make debugging easier for the user
		size_t currentLine = 1;
		size_t currentColumn = 0;

		// Block loop
		// It is escaped via a goto statement (yes its cursed but its easy)
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
					blockStart = ((depth == 0) ? currentIndex + 1 : blockStart);
					depth++;

					break;
				}

				case '}':
				{
					// If the depth is 1, then we are at the end of the top level block
					if (depth == 1)
					{
						// Adds the block to the full streamn (a vector of LexerStreamSects)
						fullStream.emplace_back(std::string_view(currentSource.data() + endOfLastBlock, blockStart - endOfLastBlock - 1), std::string_view(currentSource.data() + blockStart, currentIndex - blockStart));
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
	
	void Lexer::lexFunctionSect(const LexerStreamSect& sect)
	{}

	void Lexer::lexMacroSect(const LexerStreamSect& sect)
	{}

	Lexer::Lexer(const std::string& source) : currentSource(source)
	{
		// Checks that the source is not empty
		if (source.empty())
		{
			THROW_ERROR("Lexer must be initialized with a source code string");
		}

		// Future PASHA:
		// This can be optimized later by having a single LexerStreamSect that is set and then lexed
		// This would mean that the LexerStreamSect would be stack allocared and not heap allocated + other performance benefits
		// Splits the source into blocks
		// This allows the correct lexer function to be called on each block
		createBlocks();

		// Lexes the contents of the blocks
		// Calls the correct lexer function based on the type of the block
		for (LexerStreamSect& section : fullStream)
		{
			switch (section.type)
			{
				case SectType::FUNCTION:
				{
					lexFunctionSect(section);
					break;
				} 

				case SectType::MACRO:
				{
					lexMacroSect(section);
					break;
				}

				default:
				{
					THROW_ERROR("Unknown block type");
				}
			}
		}
	}
}
