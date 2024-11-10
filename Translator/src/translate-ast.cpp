#include <translate-ast.h>

#include <translator.h>

#include <cdt/ast.h>

#include <sstream>

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

	static const std::unordered_map<LX::Lexer::TokenType, std::string> operatorMap = 
	{
		{ LX::Lexer::TokenType::PLUS, "+"},
		{ LX::Lexer::TokenType::MINUS, "-"},
		{ LX::Lexer::TokenType::MULTIPLY, "*"},
		{ LX::Lexer::TokenType::DIVIDE, "/"},
		{ LX::Lexer::TokenType::MODULO, "%"},

		{ LX::Lexer::TokenType::PLUS_EQUALS, "+="},
		{ LX::Lexer::TokenType::MINUS_EQUALS, "-="},
		{ LX::Lexer::TokenType::MULTIPLY_EQUALS, "*="},
		{ LX::Lexer::TokenType::DIVIDE_EQUALS, "/="},

		{ LX::Lexer::TokenType::EQUALS, "=="},
		{ LX::Lexer::TokenType::NOT_EQUALS, "!="},

		{ LX::Lexer::TokenType::LESS_THAN, "<"},
		{ LX::Lexer::TokenType::LESS_THAN_EQUALS, "<="},

		{ LX::Lexer::TokenType::GREATER_THAN, ">"},
		{ LX::Lexer::TokenType::GREATER_THAN_EQUALS, ">="},

		{ LX::Lexer::TokenType::AND, "&&"},
		{ LX::Lexer::TokenType::OR, "||"},
		{ LX::Lexer::TokenType::NOT, "!"},

		{ LX::Lexer::TokenType::INCREMENT, "++"},
		{ LX::Lexer::TokenType::DECREMENT, "--"}
	};

	static inline std::string getOperator(LX::Lexer::TokenType op)
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