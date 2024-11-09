#pragma once

#include <modules/modules.h>
#include <identifier-tree.h>
#include <token.h>

#include <error.h>

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace lx
{
	enum class SectType
	{
		FUNCTION,
		SHADER,
		STRUCT,
		CLASS,
		MACRO,
		ENUM,

		UNDEFINED = -1
	};

	class Lexer;

	class LexerStreamSect
	{
		private:
			// Map that converts the info string to the correct enum type
			static const std::unordered_map<std::string, SectType> sectTypeMap;

			// Map that converts the enum type to the correct lexing function
			static const std::unordered_map<SectType, std::function<void(LexerStreamSect&)>> modules;

			// String views of the identifier and block (from the source code)

			std::string_view identifier;
			std::string_view block;

			// String to hold the return type
			// Some sections cannot have a return type so this will be void
			// Functions and shaders can also have a return type of void
			std::string info;

			// The type of the section
			// Used to call the correct lexing function
			SectType type = SectType::UNDEFINED;

		public:
			// Reference to the lexer that created this stream section
			// Allows for it to add the tokens to the correct vector of the correct lexer
			Lexer& creator;

			// Constructor that takes the identifier, block and the lexer that created it
			LexerStreamSect(std::string_view identifier, std::string_view block, Lexer& creator);

			// Function that will call the correct lexing function
			// It will then add the tokens to the correct vector of the correct lexer
			void generateTokens();

			// Debug function that will display the identifier, block and type of the stream section
			void debugDisplay() const;

			// -- Inline functions to keep some variables private -- //

			// Gets a pointer to the string view of the block
			inline const std::string_view* getBlock() const
			{
				return &block;
			}

			// Gets a pointer to the string view of the identifier
			inline const std::string_view* getIdentifier() const
			{
				return &identifier;
			}

			// Gets the info/return type of the block
			inline const std::string& getInfo() const
			{
				return info;
			}
	};

	class Lexer
	{
		private:
			// Friends the LexerStreamSect so it can access the vectors of tokens
			friend class LexerStreamSect;

			// Reference to the source code
			const std::string& currentSource;

			// Vectors of different token types (currently only function tokens)

			std::vector<Token> functionTokens;

		public:
			// Default constructor - Should not be called - Only here to stop compilaton errors
			Lexer() : currentSource("") { THROW_ERROR("Lexer must be initialized with a source code string"); }

			// Constructor that takes the source code string
			// It will do everything needed to lex the source code
			// The output for each of the token types will be stored in thier respective vectors
			Lexer(const std::string& source);

			// Default destructor - Here to look pretty
			~Lexer() = default;

			// Returns the function tokens
			inline std::vector<Token>& getFunctionTokens()
			{
				return functionTokens;
			}
	};
}