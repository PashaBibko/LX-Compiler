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

namespace LX::Parser
{
	class Parser
	{
	private:
		// Current tokens
		const std::vector<LX::Lexer::FuncToken>* currentTokens;

		// Current index in the tokens
		size_t currentIndex = 0;

		std::vector<std::unique_ptr<ASTNode>> parseBlock();

		std::unique_ptr<ASTNode> parsePrimary();

		std::unique_ptr<ASTNode> parseUnaryOperation();
		std::unique_ptr<ASTNode> parseOperation();

		std::unique_ptr<ASTNode> parseFunctionCall();

		std::unique_ptr<ASTNode> parseAssignment();

		std::unique_ptr<ASTNode> parseReturnStatement();

		std::unique_ptr<ASTNode> parseVariableDeclaration();

		std::unique_ptr<ASTNode> parseIfStatement();

		FunctionDeclaration parseFunctionDeclaration();

	public:
		Parser() {}

		void parse(const std::vector<LX::Lexer::FuncToken>& tokens, FileAST& out);
	};
}