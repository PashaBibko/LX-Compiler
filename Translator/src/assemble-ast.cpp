#include <assemble-ast.h>

#include <assembler.h>

#include <cdt/ast.h>

#include <sstream>

namespace lx
{
	void assembleIdentifier(Assembler& assembler, ASTNode* node)
	{
		assembler.out << dynamic_cast<Identifier*>(node)->name;
	}

	void assembleVariableDeclaration(Assembler& assembler, ASTNode* node)
	{
		VariableDeclaration* varDecl = dynamic_cast<VariableDeclaration*>(node);

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

	void assembleAssignment(Assembler& assembler, ASTNode* node)
	{
		Assignment* assignment = dynamic_cast<Assignment*>(node);

		assembleIdentifier(assembler, &assignment->name);

		assembler.out << " = ";
		assembler.assembleNode(assignment->val.get());
		assembler.out << ";";
	}

	static const std::unordered_map<TokenType, std::string> operatorMap = 
	{
		{TokenType::PLUS, "+"},
		{TokenType::MINUS, "-"},
		{TokenType::MULTIPLY, "*"},
		{TokenType::DIVIDE, "/"},
		{TokenType::MODULO, "%"},

		{TokenType::PLUS_EQUALS, "+="},
		{TokenType::MINUS_EQUALS, "-="},
		{TokenType::MULTIPLY_EQUALS, "*="},
		{TokenType::DIVIDE_EQUALS, "/="},

		{TokenType::EQUALS, "=="},
		{TokenType::NOT_EQUALS, "!="},

		{TokenType::LESS_THAN, "<"},
		{TokenType::LESS_THAN_EQUALS, "<="},

		{TokenType::GREATER_THAN, ">"},
		{TokenType::GREATER_THAN_EQUALS, ">="},

		{TokenType::AND, "&&"},
		{TokenType::OR, "||"},
		{TokenType::NOT, "!"},

		{TokenType::INCREMENT, "++"},
		{TokenType::DECREMENT, "--"}
	};

	static inline std::string getOperator(TokenType op)
	{
		return operatorMap.at(op);
	}

	void assembleOperation(Assembler& assembler, ASTNode* node)
	{
		Operation* operation = dynamic_cast<Operation*>(node);

		assembler.assembleNode(operation->lhs.get());
		assembler.out << " " << getOperator(operation->op) << " ";
		assembler.assembleNode(operation->rhs.get());
	}

	void assembleUnaryOperation(Assembler& assembler, ASTNode* node)
	{
		UnaryOperation* unaryOperation = dynamic_cast<UnaryOperation*>(node);

		if (unaryOperation->side == UnaryOperation::Sided::LEFT)
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

	void assembleFunctionCall(Assembler& assembler, ASTNode* node)
	{
		FunctionCall* functionCall = dynamic_cast<FunctionCall*>(node);

		if (lx::core::funcMap.find(functionCall->funcName.name) != lx::core::funcMap.end())
		{
			lx::core::funcMap[functionCall->funcName.name](functionCall, assembler);
			return;
		}

		assembleIdentifier(assembler, &functionCall->funcName);
		assembler.out << "(";

		bool firstArg = true;

		for (const std::unique_ptr<ASTNode>& arg : functionCall->args)
		{
			if (!firstArg) { assembler.out << ", "; }
			else { firstArg = false; }
			assembler.assembleNode(arg.get());
		}

		assembler.out << ")";
	}

	void assembleStringLiteral(Assembler& assembler, ASTNode* node)
	{
		assembler.out << "\"" << dynamic_cast<StringLiteral*>(node)->value << "\"";
	}

	void assembleBracketedExpression(Assembler& assembler, ASTNode* node)
	{
		BracketedExpression* bracketedExpression = dynamic_cast<BracketedExpression*>(node);

		assembler.out << "(";
		assembler.assembleNode(bracketedExpression->expr.get());
		assembler.out << ")";
	}

	void assembleIfStatement(Assembler& assembler, ASTNode* node)
	{
		IfStatement* ifStatement = dynamic_cast<IfStatement*>(node);

		while (ifStatement != nullptr)
		{
			switch (ifStatement->type)
			{
				case IfStatement::IfType::IF:
				{
					assembler.out << "if";
					break;
				}

				case IfStatement::IfType::ELSE_IF:
				{
					assembler.out << "else if";
					break;
				}

				case IfStatement::IfType::ELSE:
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
			for (const std::unique_ptr<ASTNode>& statement : ifStatement->body)
			{
				assembler.assembleNode(statement.get());
			}
			assembler.out << "\n}\n";

			if (ifStatement->next != nullptr)
			{
				ifStatement = dynamic_cast<IfStatement*>(ifStatement->next.get());
			}

			else
			{
				ifStatement = nullptr;
			}
		}
	}

	void assembleReturnStatement(Assembler& assembler, ASTNode* node)
	{
		ReturnStatement* returnStatement = dynamic_cast<ReturnStatement*>(node);

		assembler.out << "return";

		if (returnStatement->expr != nullptr)
		{
			assembler.out << " ";
			assembler.assembleNode(returnStatement->expr.get());
		}

		assembler.out << ";";
	}

	void assembleUndefined(Assembler& assembler, ASTNode* node)
	{
		throw std::runtime_error("Have fun debugging loser!");
	}
}