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

#include <common.h>

namespace LX::Lexer
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

			// Lexes the section identifier
			void lexIdentifier();

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
	};

	class Lexer
	{
		private:
			// Friends the LexerStreamSect so it can access the vectors of tokens
			friend class LexerStreamSect;

			// Reference to the source code
			const std::string& currentSource;

			//
			const bool debug;

			// Token vectors for each of the token types

			std::vector<ScopeIdentifierToken> scopeTokens;
			std::vector<FuncToken> funcTokens;

			// Debug vector to hold the stream sections
			std::vector<LexerStreamSect> sections;

		public:
			// Constructor that takes the source code string
			// It will do everything needed to lex the source code
			// The output for each of the token types will be stored in thier respective vectors
			Lexer(const std::string& source, const bool debug);

			// Default destructor - Here to look pretty
			~Lexer() = default;

			// Returns the function tokens
			inline std::vector<FuncToken>& getFunctionTokens()
			{
				return funcTokens;
			}

			// Returns the stream sections
			inline std::vector<LexerStreamSect>& getSections()
			{
				return sections;
			}
	};
}