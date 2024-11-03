#pragma once

#include <lexer/token.h>

#include <error.h>

#include <unordered_map>
#include <stdexcept>

namespace lx
{
	enum class SectType
	{
		FUNCTION,
		MACRO,

		UNDEFINED = -1
	};

	struct LexerStreamSect
	{
		static std::unordered_map<std::string, SectType> sectTypeMap;

		LexerStreamSect(std::string_view identifier, std::string_view block);

		std::string_view identifier;
		std::string_view block;

		std::string info;

		SectType type = SectType::UNDEFINED;
	};

	class Lexer
	{
		private:
			const std::string& currentSource;

			std::vector<LexerStreamSect> fullStream;

			void createBlocks();

			void lexFunctionSect(const LexerStreamSect& sect);
			void lexMacroSect(const LexerStreamSect& sect);

		public:
			Lexer() : currentSource("")
			{
				THROW_ERROR("Lexer must be initialized with a source code string");
			}

			Lexer(const std::string&  source);

			~Lexer()
			{}
	};
}