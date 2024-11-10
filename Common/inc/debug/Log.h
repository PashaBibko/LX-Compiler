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

#include <cdt/token.h>
#include <cdt/ast.h>

#define TOKEN_CASE(tokenCase) case LX::Lexer::tokenCase: std::cout << #tokenCase << ": " << token.value << std::endl; break;

namespace LX::Debug
{
	inline void Log(const LX::Lexer::Token& token)
	{
		switch (token.type)
		{
			TOKEN_CASE(TokenType::IDENTIFIER)
				TOKEN_CASE(TokenType::STRING_LITERAL)
				TOKEN_CASE(TokenType::INT_DEC)
				TOKEN_CASE(TokenType::STR_DEC)
				TOKEN_CASE(TokenType::CONST)
				TOKEN_CASE(TokenType::IF)
				TOKEN_CASE(TokenType::ELIF)
				TOKEN_CASE(TokenType::ELSE)
				TOKEN_CASE(TokenType::FOR)
				TOKEN_CASE(TokenType::WHILE)
				TOKEN_CASE(TokenType::BREAK)
				TOKEN_CASE(TokenType::CONTINUE)
				TOKEN_CASE(TokenType::RETURN)
				TOKEN_CASE(TokenType::FUNCTION)
				TOKEN_CASE(TokenType::PROCEDURE)
				TOKEN_CASE(TokenType::PLUS)
				TOKEN_CASE(TokenType::MINUS)
				TOKEN_CASE(TokenType::MULTIPLY)
				TOKEN_CASE(TokenType::DIVIDE)
				TOKEN_CASE(TokenType::MODULO)
				TOKEN_CASE(TokenType::PLUS_EQUALS)
				TOKEN_CASE(TokenType::MINUS_EQUALS)
				TOKEN_CASE(TokenType::MULTIPLY_EQUALS)
				TOKEN_CASE(TokenType::DIVIDE_EQUALS)
				TOKEN_CASE(TokenType::POWER)
				TOKEN_CASE(TokenType::INCREMENT)
				TOKEN_CASE(TokenType::DECREMENT)
				TOKEN_CASE(TokenType::EQUALS)
				TOKEN_CASE(TokenType::LESS_THAN)
				TOKEN_CASE(TokenType::GREATER_THAN)
				TOKEN_CASE(TokenType::LESS_THAN_EQUALS)
				TOKEN_CASE(TokenType::GREATER_THAN_EQUALS)
				TOKEN_CASE(TokenType::NOT_EQUALS)
				TOKEN_CASE(TokenType::AND)
				TOKEN_CASE(TokenType::NOT)
				TOKEN_CASE(TokenType::OR)
				TOKEN_CASE(TokenType::ASSIGN)
				TOKEN_CASE(TokenType::COMMA)
				TOKEN_CASE(TokenType::SEMICOLON)
				TOKEN_CASE(TokenType::COLON)
				TOKEN_CASE(TokenType::DOT)
				TOKEN_CASE(TokenType::ARROW)
				TOKEN_CASE(TokenType::DOUBLE_ARROW)
				TOKEN_CASE(TokenType::DOUBLE_COLON)
				TOKEN_CASE(TokenType::LEFT_PAREN)
				TOKEN_CASE(TokenType::RIGHT_PAREN)
				TOKEN_CASE(TokenType::LEFT_BRACE)
				TOKEN_CASE(TokenType::RIGHT_BRACE)
				TOKEN_CASE(TokenType::LEFT_BRACKET)
				TOKEN_CASE(TokenType::RIGHT_BRACKET)
				TOKEN_CASE(TokenType::END_OF_FILE)
				TOKEN_CASE(TokenType::UNDEFINED)
		}
	}

	inline void Log(const std::vector<LX::Lexer::Token>& tokens)
	{
		int counter = 0;

		for (const LX::Lexer::Token& token : tokens)
		{
			std::cout << counter << ": ";
			Log(token);
			counter++;
		}
	}

	void Log(const std::unique_ptr<LX::Parser::ASTNode>& node, int depth);

	inline void LogA(std::unique_ptr<LX::Parser::Assignment>& assignment, int depth)
	{
		std::cout << std::string(depth, '\t') << "Assignment: " << assignment->name.name << std::endl;
		Log(assignment->val, depth + 1);
	}

	inline void Log(LX::Parser::IfStatement* ifStatement, int depth)
	{
		switch (ifStatement->type)
		{
			case LX::Parser::IfStatement::IfType::IF:
			{
				std::cout << std::string(depth, '\t') << "If Statement: " << std::endl;

				Log(ifStatement->condition, depth + 1);

				std::cout << std::string(depth, '\t') << "Then: " << std::endl;

				for (std::unique_ptr<LX::Parser::ASTNode>& statement : ifStatement->body)
				{
					Log(statement, depth + 1);
				}

				break;
			}

			case LX::Parser::IfStatement::IfType::ELSE_IF:
			{
				std::cout << std::string(depth, '\t') << "Else If Statement: " << std::endl;

				Log(ifStatement->condition, depth + 1);

				std::cout << std::string(depth, '\t') << "Then: " << std::endl;

				for (std::unique_ptr<LX::Parser::ASTNode>& statement : ifStatement->body)
				{
					Log(statement, depth + 1);
				}

				break;
			}

			case LX::Parser::IfStatement::IfType::ELSE:
			{
				std::cout << std::string(depth, '\t') << "Else Statement: " << std::endl;

				for (std::unique_ptr<LX::Parser::ASTNode>& statement : ifStatement->body)
				{
					Log(statement, depth + 1);
				}

				break;
			}
		}

		if (ifStatement->next != nullptr)
		{
			Log(ifStatement->next.get(), depth);
		}
	}

	inline void Log(LX::Parser::FunctionDeclaration* funcDecl, int depth)
	{
		std::cout << std::string(depth, '\t') << "Function Declaration: " << funcDecl->name.name << std::endl;

		for (LX::Parser::Identifier& returnType : funcDecl->returnTypes)
		{
			std::cout << std::string(depth, '\t') << "Return Type: " << returnType.name << std::endl;
		}

		for (std::unique_ptr<LX::Parser::ASTNode>& arg : funcDecl->args)
		{
			Log(arg, depth + 1);
		}

		for (std::unique_ptr<LX::Parser::ASTNode>& statement : funcDecl->body)
		{
			Log(statement, depth + 1);
		}
	}

	inline void Log(const std::unique_ptr<LX::Parser::ASTNode>& node, int depth)
	{
		switch (node->type)
		{
			case LX::Parser::ASTNode::NodeType::IDENTIFIER:
			{
				LX::Parser::Identifier* identifier = static_cast<LX::Parser::Identifier*>(node.get());

				std::cout << std::string(depth, '\t') << "Identifier: " << identifier->name << std::endl;

				return;
			}

			case LX::Parser::ASTNode::NodeType::VARIABLE_DECLARATION:
			{
				LX::Parser::VariableDeclaration* varDecl = static_cast<LX::Parser::VariableDeclaration*>(node.get());

				std::cout << std::string(depth, '\t') << "Variable Declaration: type {" << varDecl->varType.name << "} name {" << varDecl->name.name << "}" << std::endl;
				std::cout << std::string(depth + 1, '\t') << "Const: " << (varDecl->isConst() ? "true" : "false") << std::endl;

				if (varDecl->val != nullptr)
				{
					LogA(varDecl->val, depth + 1);
				}

				return;
			}

			case LX::Parser::ASTNode::NodeType::ASSIGNMENT:
			{
				LX::Parser::Assignment* assignment = static_cast<LX::Parser::Assignment*>(node.get());

				std::cout << std::string(depth, '\t') << "Assignment: " << assignment->name.name << std::endl;
				Log(assignment->val, depth + 1);

				return;
			}

			case LX::Parser::ASTNode::NodeType::OPERATION:
			{
				LX::Parser::Operation* operation = static_cast<LX::Parser::Operation*>(node.get());

				Log(operation->lhs, depth + 1);
				std::cout << std::string(depth, '\t') << "Operation: ";
				Log(operation->op);
				Log(operation->rhs, depth + 1);

				return;
			}

			case LX::Parser::ASTNode::NodeType::UNARY_OPERATION:
			{
				LX::Parser::UnaryOperation* unaryOperation = static_cast<LX::Parser::UnaryOperation*>(node.get());

				std::cout << std::string(depth, '\t') << "Unary Operation: ";
				Log(unaryOperation->op);
				Log(unaryOperation->val, depth + 1);

				std::cout << std::string(depth, '\t') << (unaryOperation->side == LX::Parser::UnaryOperation::Sided::LEFT ? "Left" : "Right") << " sided" << std::endl;

				return;
			}

			case LX::Parser::ASTNode::NodeType::FUNCTION_CALL:
			{
				LX::Parser::FunctionCall* functionCall = static_cast<LX::Parser::FunctionCall*>(node.get());

				std::cout << std::string(depth, '\t') << "Function Call: " << functionCall->funcName.name << std::endl;

				for (std::unique_ptr<LX::Parser::ASTNode>& arg : functionCall->args)
				{
					Log(arg, depth + 1);
				}

				return;
			}

			case LX::Parser::ASTNode::NodeType::STRING_LITERAL:
			{
				LX::Parser::StringLiteral* stringLiteral = static_cast<LX::Parser::StringLiteral*>(node.get());

				std::cout << std::string(depth, '\t') << "String Literal: " << stringLiteral->value << std::endl;

				return;
			}

			case LX::Parser::ASTNode::NodeType::IF_STATEMENT:
			{
				Log(static_cast<LX::Parser::IfStatement*>(node.get()), depth);

				return;
			}

			case LX::Parser::ASTNode::NodeType::RETURN_STATEMENT:
			{
				LX::Parser::ReturnStatement* returnStatement = static_cast<LX::Parser::ReturnStatement*>(node.get());

				std::cout << std::string(depth, '\t') << "Return Statement: " << std::endl;

				Log(returnStatement->expr, depth + 1);

				return;
			}

			case LX::Parser::ASTNode::NodeType::BRACKETED_EXPRESSION:
			{
				LX::Parser::BracketedExpression* bracketedExpression = static_cast<LX::Parser::BracketedExpression*>(node.get());

				std::cout << std::string(depth, '\t') << "Bracketed Expression: " << std::endl;

				Log(bracketedExpression->expr, depth + 1);

				return;
			}

			default:
			{
				std::cout << std::string(depth, '\t') << "Undefined AST node: " << (int)node->type << std::endl;

				return;
			}
		}
	}

	inline void Log(LX::Parser::FileAST& AST)
	{
		for (LX::Parser::FunctionDeclaration& funcDecl : AST.functions)
		{
			Log(&funcDecl, 0);
		}

	}
}