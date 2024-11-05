#pragma once

#include <lexer/lexer.h>

#include <error.h>

namespace lx
{
	// Forward declaration
	class LexerStreamSect;

	// Lexer module functions

	void lexFunction(LexerStreamSect& sect);
	void lexShader(LexerStreamSect& sect);
	void lexMacro(LexerStreamSect& sect);
	void lexClass(LexerStreamSect& sect);
	void lexEnum(LexerStreamSect& sect);
}