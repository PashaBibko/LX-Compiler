#pragma once

#include <string>
namespace lx
{
	enum class TokenType : short
	{
		// Identifiers //
		IDENTIFIER, // ( Word )

		// Literals //
		STRING_LITERAL,

		// Var Types //

		INT_DEC,
		STR_DEC,

		// Var Modifiers //

		CONST,

		// Control flow //
		// These are not identifiers because thier behavior cannot be changed at compile/runtime //

		// Simple control flow
		IF,
		ELIF,
		ELSE,

		// Loops
		FOR,
		WHILE,

		// Goto statements
		BREAK,
		CONTINUE,
		RETURN,

		// Functions
		FUNCTION,
		PROCEDURE,

		// Operators //

		// Simple arithmetic operators
		PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,

		// Complex arithmetic operators
		PLUS_EQUALS,
		MINUS_EQUALS,
		MULTIPLY_EQUALS,
		DIVIDE_EQUALS,

		POWER,

		// Increment and decrement operators
		INCREMENT,
		DECREMENT,

		// Comparison operators
		EQUALS,
		NOT_EQUALS,

		GREATER_THAN, // >
		GREATER_THAN_EQUALS, // >=

		LESS_THAN, // <
		LESS_THAN_EQUALS, // <=

		// Logical operators
		AND,
		OR,
		NOT,

		// Assignment operators
		ASSIGN,

		// Separators //

		// Singlechar
		COMMA,
		SEMICOLON,
		COLON,
		DOT,

		// Multichar
		ARROW, // ->
		DOUBLE_ARROW, // =>
		DOUBLE_COLON, // ::

		// Brackets //

		// ()
		LEFT_PAREN,
		RIGHT_PAREN,

		// {}
		LEFT_BRACE,
		RIGHT_BRACE,

		// []
		LEFT_BRACKET,
		RIGHT_BRACKET,

		// Only occurs at the end of a file
		END_OF_FILE,

		// Causes an error if not defined
		UNDEFINED
	};

	class Token
	{
		public:
			Token() = default;
			Token(TokenType type, const std::string value = "") : value(value), type(type) {}

			std::string value;
			TokenType type = TokenType::UNDEFINED;
	};

}