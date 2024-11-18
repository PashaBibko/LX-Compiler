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

#pragma once

#include <std-libs.h>

namespace LX::Lexer
{
	struct ScopeIdentifierToken
	{
		enum Type
		{
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

			// <>
			LEFT_ANGLE_BRACKET,
			RIGHT_ANGLE_BRACKET,

			// Punctuation //

			COMMA,

			// General //

			IDENTIFIER,

			// Keywords

			DECORATOR,
			TYPENAME,
			UNIFORM,

			FUNCTION,
			ENUM,
			CLASS,
			STRUCT,
			SHADER,
			MACRO,

			// End of identifier //
			END_OF_IDENTIFIER
		};

		ScopeIdentifierToken() = default;
		ScopeIdentifierToken(Type t, const std::string& v) : type(t), value(v) {}

		std::string value;
		Type type;
	};

	struct FuncToken
	{
		enum Type
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

			// Only occurs at the end of a scope
			END_OF_SCOPE,

			// Causes an error if not defined
			UNDEFINED
		};

		FuncToken() = default;
		FuncToken(Type t, const std::string& v = "") : type(t), value(v) {}

		std::string value;
		Type type;
	};

	struct ClassToken
	{
		enum Type
		{
		};

		ClassToken() = default;
		ClassToken(Type t, const std::string& v) : type(t), value(v) {}

		std::string value;
		Type type;
	};

	struct EnumToken
	{
		enum Type
		{
		};

		EnumToken() = default;
		EnumToken(Type t, const std::string& v) : type(t), value(v) {}

		std::string value;
		Type type;
	};

	struct MacroToken
	{
		enum Type
		{
		};

		MacroToken() = default;
		MacroToken(Type t, const std::string& v) : type(t), value(v) {}

		std::string value;
		Type type;
	};

	struct ShaderToken
	{
		enum Type
		{
		};

		ShaderToken() = default;
		ShaderToken(Type t, const std::string& v) : type(t), value(v) {}

		std::string value;
		Type type;
	};
}