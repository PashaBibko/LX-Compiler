#pragma once

#include <cdt/token.h>
#include <cdt/ast.h>

#include <macro/dll-func.h>

#include <unordered_map>

namespace LX::Common
{
	struct LexerOutput
	{
		std::vector<LX::Lexer::Token> funcTokens;
	};

	struct ParserOutput
	{
		LX::Parser::FileAST ast;
	};

	struct DataStorage
	{
		DataStorage() { std::cout << "DataStorage created" << std::endl; }

		std::unordered_map<unsigned int, LexerOutput> lexerOutputs;
		std::unordered_map<unsigned int, ParserOutput> parserOutputs;
	};

	extern DLL_EXPORT DataStorage storage;
}
