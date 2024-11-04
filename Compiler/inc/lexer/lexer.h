#pragma once

#include <lexer/modules/modules.h>
#include <lexer/token.h>

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
			static const std::unordered_map<std::string, SectType> sectTypeMap;
			static const std::unordered_map<SectType, std::function<void(LexerStreamSect&)>> modules;

			std::string_view identifier;
			std::string_view block;

			std::string info;

			SectType type = SectType::UNDEFINED;

			const Lexer& creator;

		public:
			LexerStreamSect(std::string_view identifier, std::string_view block, const Lexer& creator);

			void generateTokens();

			void debugDisplay() const;
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
			inline const std::vector<Token>& getFunctionTokens()
			{
				return functionTokens;
			}
	};
}