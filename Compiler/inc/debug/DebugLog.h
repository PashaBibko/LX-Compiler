#pragma once

#include <lexer/token.h>
#include <parser/ast.h>

#include <iostream>
#include <vector>

#define TOKEN_CASE(tokenCase) case tokenCase: std::cout << #tokenCase << ": " << token.value << std::endl; break;

inline void DebugLog(const Token& token)
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

inline void DebugLog(const std::vector<Token>& tokens)
{
	int counter = 0;

	for (const Token& token : tokens)
	{
		std::cout << counter << ": ";
		DebugLog(token);
		counter++;
	}
}

void DebugLog(const std::unique_ptr<ASTNode>& node, int depth);

inline void DebugLogA(std::unique_ptr<Assignment>& assignment, int depth)
{
	std::cout << std::string(depth, '\t') << "Assignment: " << assignment->name.name << std::endl;
	DebugLog(assignment->val, depth + 1);
}

inline void DebugLog(const std::unique_ptr<ASTNode>& node, int depth)
{
	switch (node->type)
	{
		case ASTNode::NodeType::IDENTIFIER:
		{
			Identifier* identifier = static_cast<Identifier*>(node.get());

			std::cout << std::string(depth, '\t') << "Identifier: " << identifier->name << std::endl;

			return;
		}

		case ASTNode::NodeType::VARIABLE_DECLARATION:
		{
			VariableDeclaration* varDecl = static_cast<VariableDeclaration*>(node.get());

			std::cout << std::string(depth, '\t') << "Variable Declaration: type {" << varDecl->varType.name << "} name {" << varDecl->name.name << "}" << std::endl;
			std::cout << std::string(depth + 1, '\t') << "Const: " << (varDecl->isConst() ? "true" : "false") << std::endl;

			if (varDecl->val != nullptr)
			{
				DebugLogA(varDecl->val, depth + 1);
			}

			return;
		}

		case ASTNode::NodeType::ASSIGNMENT:
		{
			Assignment* assignment = static_cast<Assignment*>(node.get());

			std::cout << std::string(depth, '\t') << "Assignment: " << assignment->name.name << std::endl;
			DebugLog(assignment->val, depth + 1);

			return;
		}

		case ASTNode::NodeType::OPERATION:
		{
			Operation* operation = static_cast<Operation*>(node.get());

			DebugLog(operation->lhs, depth + 1);
			std::cout << std::string(depth, '\t') << "Operation: ";
			DebugLog(operation->op);
			DebugLog(operation->rhs, depth + 1);

			return;
		}

		case ASTNode::NodeType::UNARY_OPERATION:
		{
			UnaryOperation* unaryOperation = static_cast<UnaryOperation*>(node.get());

			std::cout << std::string(depth, '\t') << "Unary Operation: ";
			DebugLog(unaryOperation->op);
			DebugLog(unaryOperation->val, depth + 1);

			std::cout << std::string(depth, '\t') << (unaryOperation->side == UnaryOperation::Sided::LEFT ? "Left" : "Right") << " sided" << std::endl;

			return;
		}

		case ASTNode::NodeType::FUNCTION_CALL:
		{
			FunctionCall* functionCall = static_cast<FunctionCall*>(node.get());

			std::cout << std::string(depth, '\t') << "Function Call: " << functionCall->funcName.name << std::endl;

			for (std::unique_ptr<ASTNode>& arg : functionCall->args)
			{
				DebugLog(arg, depth + 1);
			}

			return;
		}

		case ASTNode::NodeType::STRING_LITERAL:
		{
			StringLiteral* stringLiteral = static_cast<StringLiteral*>(node.get());

			std::cout << std::string(depth, '\t') << "String Literal: " << stringLiteral->value << std::endl;

			return;
		}

		case ASTNode::NodeType::UNDEFINED:
		{
			std::cout << std::string(depth, '\t') << "Undefined: " << (int)node->type << std::endl;

			return;
		}
	}
}

inline void DebugLog(FileAST& AST)
{
	for (const std::unique_ptr<ASTNode>& node : AST.script)
	{
		DebugLog(node, 0);
	}
}
