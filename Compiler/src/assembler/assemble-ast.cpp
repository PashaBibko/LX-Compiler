#include <assembler/assemble-ast.h>

#include <assembler/assembler.h>

#include <parser/parser.h>

#include <sstream>

namespace lx
{
	void assembleIdentifier(Assembler& assembler, ASTNode* node)
	{
		Identifier* identifier = dynamic_cast<Identifier*>(node);

		assembler.out << identifier->name;
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
			assembler.out << " = ";
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

	static std::string getOperator(TokenType op)
	{
		switch (op)
		{
			case TokenType::PLUS: return "+";
			case TokenType::MINUS: return "-";
			case TokenType::MULTIPLY: return "*";
			case TokenType::DIVIDE: return "/";
			case TokenType::MODULO: return "%";

			case TokenType::PLUS_EQUALS: return "+=";
			case TokenType::MINUS_EQUALS: return "-=";
			case TokenType::MULTIPLY_EQUALS: return "*=";
			case TokenType::DIVIDE_EQUALS: return "/=";

			case TokenType::EQUALS: return "==";
			case TokenType::NOT_EQUALS: return "!=";
			case TokenType::LESS_THAN: return "<";
			case TokenType::GREATER_THAN: return ">";
			case TokenType::LESS_THAN_EQUALS: return "<=";
			case TokenType::GREATER_THAN_EQUALS: return ">=";

			case TokenType::AND: return "&&";
			case TokenType::OR: return "||";
			case TokenType::NOT: return "!";

			case TokenType::INCREMENT: return "++";
			case TokenType::DECREMENT: return "--";

			default: throw std::runtime_error("Invalid operator");
		}
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
			std::cout << "Found function: " << functionCall->funcName.name << std::endl;
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
		StringLiteral* stringLiteral = dynamic_cast<StringLiteral*>(node);

		assembler.out << "\"" << stringLiteral->value << "\"";
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
					assembler.out << "if (";
					assembler.assembleNode(ifStatement->condition.get());
					assembler.out << ")\n{\n";

					for (const std::unique_ptr<ASTNode>& statement : ifStatement->body)
					{
						assembler.assembleNode(statement.get());
					}

					assembler.out << "\n}\n";

					ifStatement = ifStatement->next.get();
					break;
				}

				case IfStatement::IfType::ELSE_IF:
				{
					assembler.out << "else if (";
					assembler.assembleNode(ifStatement->condition.get());
					assembler.out << ")\n{\n";

					for (const std::unique_ptr<ASTNode>& statement : ifStatement->body)
					{
						assembler.assembleNode(statement.get());
					}

					assembler.out << "\n}\n";

					ifStatement = ifStatement->next.get();
					break;
				}

				case IfStatement::IfType::ELSE:
				{
					assembler.out << "else\n{\n";

					for (const std::unique_ptr<ASTNode>& statement : ifStatement->body)
					{
						assembler.assembleNode(statement.get());
					}

					assembler.out << "\n}\n";

					ifStatement = nullptr;
					break;
				}
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