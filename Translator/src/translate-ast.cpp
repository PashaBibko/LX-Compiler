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

#include <translate-ast.h>

#include <common.h>

#include <translator.h>

namespace LX::Translator
{
	void assembleIdentifier(Translator& translator, LX::Parser::ASTNode* node)
	{
		translator.out << dynamic_cast<LX::Parser::Identifier*>(node)->name;
	}

	void assembleVariableDeclaration(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::VariableDeclaration* varDecl = dynamic_cast<LX::Parser::VariableDeclaration*>(node);

		// Variable modifiers

		if (varDecl->isConst()) { translator.out << "const "; }

		// Variable type

		if (varDecl->varType.name == "string")
		{
			translator.includes.insert("string");
			translator.out << "std::string ";
		}

		else
		{
			translator.out << varDecl->varType.name << " ";
		}

		// Variable name

		assembleIdentifier(translator, &varDecl->name);

		// Variable value

		if (varDecl->val != nullptr)
		{
			assembleAssignment(translator, varDecl->val.get());
		}
	}

	void assembleAssignment(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::Assignment* assignment = dynamic_cast<LX::Parser::Assignment*>(node);

		assembleIdentifier(translator, &assignment->name);

		translator.out << " = ";
		translator.assembleNode(assignment->val.get());
		translator.out << ";";
	}

	static const std::unordered_map<LX::Lexer::FuncToken::Type, std::string> operatorMap = 
	{
		{ LX::Lexer::FuncToken::PLUS, "+"},
		{ LX::Lexer::FuncToken::MINUS, "-"},
		{ LX::Lexer::FuncToken::MULTIPLY, "*"},
		{ LX::Lexer::FuncToken::DIVIDE, "/"},
		{ LX::Lexer::FuncToken::MODULO, "%"},

		{ LX::Lexer::FuncToken::PLUS_EQUALS, "+="},
		{ LX::Lexer::FuncToken::MINUS_EQUALS, "-="},
		{ LX::Lexer::FuncToken::MULTIPLY_EQUALS, "*="},
		{ LX::Lexer::FuncToken::DIVIDE_EQUALS, "/="},

		{ LX::Lexer::FuncToken::EQUALS, "=="},
		{ LX::Lexer::FuncToken::NOT_EQUALS, "!="},

		{ LX::Lexer::FuncToken::LESS_THAN, "<"},
		{ LX::Lexer::FuncToken::LESS_THAN_EQUALS, "<="},

		{ LX::Lexer::FuncToken::GREATER_THAN, ">"},
		{ LX::Lexer::FuncToken::GREATER_THAN_EQUALS, ">="},

		{ LX::Lexer::FuncToken::AND, "&&"},
		{ LX::Lexer::FuncToken::OR, "||"},
		{ LX::Lexer::FuncToken::NOT, "!"},

		{ LX::Lexer::FuncToken::INCREMENT, "++"},
		{ LX::Lexer::FuncToken::DECREMENT, "--"}
	};

	static inline std::string getOperator(LX::Lexer::FuncToken::Type op)
	{
		return operatorMap.at(op);
	}

	void assembleOperation(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::Operation* operation = dynamic_cast<LX::Parser::Operation*>(node);

		translator.assembleNode(operation->lhs.get());
		translator.out << " " << getOperator(operation->op) << " ";
		translator.assembleNode(operation->rhs.get());
	}

	void assembleUnaryOperation(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::UnaryOperation* unaryOperation = dynamic_cast<LX::Parser::UnaryOperation*>(node);

		if (unaryOperation->side == LX::Parser::UnaryOperation::Sided::LEFT)
		{
			translator.out << getOperator(unaryOperation->op) << " ";
			translator.assembleNode(unaryOperation->val.get());
		}

		else
		{
			translator.assembleNode(unaryOperation->val.get());
			translator.out << " " << getOperator(unaryOperation->op);
		}
	}

	void assembleFunctionCall(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::FunctionCall* functionCall = dynamic_cast<LX::Parser::FunctionCall*>(node);

		if (Core::funcMap.find(functionCall->funcName.name) != Core::funcMap.end())
		{
			Core::funcMap[functionCall->funcName.name](functionCall, translator);
			return;
		}

		assembleIdentifier(translator, &functionCall->funcName);
		translator.out << "(";

		bool firstArg = true;

		for (const std::unique_ptr<LX::Parser::ASTNode>& arg : functionCall->args)
		{
			if (!firstArg) { translator.out << ", "; }
			else { firstArg = false; }
			translator.assembleNode(arg.get());
		}

		translator.out << ")";
	}

	void assembleStringLiteral(Translator& translator, LX::Parser::ASTNode* node)
	{
		translator.out << "\"" << dynamic_cast<LX::Parser::StringLiteral*>(node)->value << "\"";
	}

	void assembleBracketedExpression(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::BracketedExpression* bracketedExpression = dynamic_cast<LX::Parser::BracketedExpression*>(node);

		translator.out << "(";
		translator.assembleNode(bracketedExpression->expr.get());
		translator.out << ")";
	}

	void assembleIfStatement(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::IfStatement* ifStatement = dynamic_cast<LX::Parser::IfStatement*>(node);

		while (ifStatement != nullptr)
		{
			switch (ifStatement->type)
			{
				case LX::Parser::IfStatement::IfType::IF:
				{
					translator.out << "if";
					break;
				}

				case LX::Parser::IfStatement::IfType::ELSE_IF:
				{
					translator.out << "else if";
					break;
				}

				case LX::Parser::IfStatement::IfType::ELSE:
				{
					translator.out << "else";
					break;
				}
			}

			if (ifStatement->condition != nullptr)
			{
				translator.out << "(";
				translator.assembleNode(ifStatement->condition.get());
				translator.out << ")";
			}

			translator.out << "\n{\n";
			for (const std::unique_ptr<LX::Parser::ASTNode>& statement : ifStatement->body)
			{
				translator.assembleNode(statement.get());
			}
			translator.out << "\n}\n";

			if (ifStatement->next != nullptr)
			{
				ifStatement = dynamic_cast<LX::Parser::IfStatement*>(ifStatement->next.get());
			}

			else
			{
				ifStatement = nullptr;
			}
		}
	}

	void assembleReturnStatement(Translator& translator, LX::Parser::ASTNode* node)
	{
		LX::Parser::ReturnStatement* returnStatement = dynamic_cast<LX::Parser::ReturnStatement*>(node);

		translator.out << "return";

		if (returnStatement->expr != nullptr)
		{
			translator.out << " ";
			translator.assembleNode(returnStatement->expr.get());
		}

		translator.out << ";";
	}

	void assembleUndefined(Translator& translator, LX::Parser::ASTNode* node)
	{
		throw std::runtime_error("Have fun debugging loser!");
	}
}