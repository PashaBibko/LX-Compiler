#include <assemble-ast.h>

#include <assembler.h>

#include <cdt/ast.h>

#include <sstream>

namespace LX::Translator
{
	void assembleIdentifier(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		assembler.out << dynamic_cast<LX::Parser::Identifier*>(node)->name;
	}

	void assembleVariableDeclaration(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::VariableDeclaration* varDecl = dynamic_cast<LX::Parser::VariableDeclaration*>(node);

		// Variable modifiers

		if (varDecl->isConst()) { assembler.out << "const "; }

		// Variable type

		if (varDecl->varType.name == "string")
		{
			assembler.includes.insert("string");
			assembler.out << "std::string ";
		}

		else
		{
			assembler.out << varDecl->varType.name << " ";
		}

		// Variable name

		assembleIdentifier(assembler, &varDecl->name);

		// Variable value

		if (varDecl->val != nullptr)
		{
			assembleAssignment(assembler, varDecl->val.get());
		}
	}

	void assembleAssignment(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::Assignment* assignment = dynamic_cast<LX::Parser::Assignment*>(node);

		assembleIdentifier(assembler, &assignment->name);

		assembler.out << " = ";
		assembler.assembleNode(assignment->val.get());
		assembler.out << ";";
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

	void assembleOperation(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::Operation* operation = dynamic_cast<LX::Parser::Operation*>(node);

		assembler.assembleNode(operation->lhs.get());
		assembler.out << " " << getOperator(operation->op) << " ";
		assembler.assembleNode(operation->rhs.get());
	}

	void assembleUnaryOperation(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::UnaryOperation* unaryOperation = dynamic_cast<LX::Parser::UnaryOperation*>(node);

		if (unaryOperation->side == LX::Parser::UnaryOperation::Sided::LEFT)
		{
			assembler.out << getOperator(unaryOperation->op) << " ";
			assembler.assembleNode(unaryOperation->val.get());
		}

		else
		{
			assembler.assembleNode(unaryOperation->val.get());
			assembler.out << " " << getOperator(unaryOperation->op);
		}
	}

	void assembleFunctionCall(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::FunctionCall* functionCall = dynamic_cast<LX::Parser::FunctionCall*>(node);

		if (Core::funcMap.find(functionCall->funcName.name) != Core::funcMap.end())
		{
			Core::funcMap[functionCall->funcName.name](functionCall, assembler);
			return;
		}

		assembleIdentifier(assembler, &functionCall->funcName);
		assembler.out << "(";

		bool firstArg = true;

		for (const std::unique_ptr<LX::Parser::ASTNode>& arg : functionCall->args)
		{
			if (!firstArg) { assembler.out << ", "; }
			else { firstArg = false; }
			assembler.assembleNode(arg.get());
		}

		assembler.out << ")";
	}

	void assembleStringLiteral(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		assembler.out << "\"" << dynamic_cast<LX::Parser::StringLiteral*>(node)->value << "\"";
	}

	void assembleBracketedExpression(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::BracketedExpression* bracketedExpression = dynamic_cast<LX::Parser::BracketedExpression*>(node);

		assembler.out << "(";
		assembler.assembleNode(bracketedExpression->expr.get());
		assembler.out << ")";
	}

	void assembleIfStatement(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::IfStatement* ifStatement = dynamic_cast<LX::Parser::IfStatement*>(node);

		while (ifStatement != nullptr)
		{
			switch (ifStatement->type)
			{
				case LX::Parser::IfStatement::IfType::IF:
				{
					assembler.out << "if";
					break;
				}

				case LX::Parser::IfStatement::IfType::ELSE_IF:
				{
					assembler.out << "else if";
					break;
				}

				case LX::Parser::IfStatement::IfType::ELSE:
				{
					assembler.out << "else";
					break;
				}
			}

			if (ifStatement->condition != nullptr)
			{
				assembler.out << "(";
				assembler.assembleNode(ifStatement->condition.get());
				assembler.out << ")";
			}

			assembler.out << "\n{\n";
			for (const std::unique_ptr<LX::Parser::ASTNode>& statement : ifStatement->body)
			{
				assembler.assembleNode(statement.get());
			}
			assembler.out << "\n}\n";

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

	void assembleReturnStatement(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		LX::Parser::ReturnStatement* returnStatement = dynamic_cast<LX::Parser::ReturnStatement*>(node);

		assembler.out << "return";

		if (returnStatement->expr != nullptr)
		{
			assembler.out << " ";
			assembler.assembleNode(returnStatement->expr.get());
		}

		assembler.out << ";";
	}

	void assembleUndefined(Assembler& assembler, LX::Parser::ASTNode* node)
	{
		throw std::runtime_error("Have fun debugging loser!");
	}
}