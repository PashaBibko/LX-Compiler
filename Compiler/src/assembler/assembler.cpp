#include <assembler/assembler.h>

#include <assembler/lx-core.h>
#include <parser/parser.h>

namespace lx
{
	std::string Assembler::assembleFunctionCall(FunctionCall* call)
	{
		// Checks if the function is in the core function map
		if (lx::core::funcMap.find(call->funcName.name) != lx::core::funcMap.end())
			return lx::core::funcMap[call->funcName.name](call, *this);

		// If the function is not in the core function map, it is a user defined function
		std::string output = call->funcName.name + "(";

		bool firstArg = true;

		// Adds the arguments to the output
		for (std::unique_ptr<ASTNode>& arg : call->args)
		{
			if (!firstArg)
				output = output + ", ";

			output = output + assembleNode(arg);

			firstArg = false;
		}

		// Adds the closing bracket with the return statement
		return output + ");";
	}

	inline std::string Assembler::assembleIdentifier(Identifier* id)
	{
		return id->name;
	}

	inline std::string Assembler::assembleStringLiteral(StringLiteral* str)
	{
		return "\"" + str->value + "\"";
	}

	inline std::string Assembler::assembleAssignment(Assignment* assign)
	{
		return Assembler::assembleIdentifier(&assign->name) + " = " + assembleNode(assign->val) + ";";
	}

	std::string Assembler::assembleVarMods(VariableDeclaration* var)
	{
		std::string out = "";

		if (var->isConst())
			out = out + "const ";

		return out;
	}

	std::string Assembler::assembleVarDecType(Identifier* t)
	{
		if (t->name == "int") { return "int"; }

		else if (t->name == "string")
		{
			includes["string"] = true;
			return "std::string";
		}

		else
			return t->name;
	}

	std::string Assembler::assembleVarDec(VariableDeclaration* var)
	{
		if (var->val == nullptr)
			return assembleVarMods(var) + assembleVarDecType(&var->varType) + " " + var->name.name + ";";

		else
			return assembleVarMods(var) + assembleVarDecType(&var->varType) + " " + var->name.name + assembleAssignment(var->val.get());
	}

	std::string Assembler::assembleOperand(TokenType op)
	{
		switch (op)
		{
		case TokenType::PLUS: { return "+"; }
		case TokenType::MINUS: { return "-"; }
		case TokenType::MULTIPLY: { return "*"; }
		case TokenType::DIVIDE: { return "/"; }
		case TokenType::MODULO: { return "%"; }

		case TokenType::PLUS_EQUALS: { return "+="; }
		case TokenType::MINUS_EQUALS: { return "-="; }
		case TokenType::MULTIPLY_EQUALS: { return "*="; }
		case TokenType::DIVIDE_EQUALS: { return "/="; }

		case TokenType::EQUALS: { return "=="; }
		case TokenType::LESS_THAN: { return "<"; }
		case TokenType::GREATER_THAN: { return ">"; }
		case TokenType::LESS_THAN_EQUALS: { return "<="; }
		case TokenType::GREATER_THAN_EQUALS: { return ">="; }
		case TokenType::NOT_EQUALS: { return "!="; }

		case TokenType::AND: { return "&&"; }
		case TokenType::NOT: { return "!"; }
		case TokenType::OR: { return "||"; }

		case TokenType::INCREMENT: { return "++"; }
		case TokenType::DECREMENT: { return "--"; }

		default:
		{
			std::cerr << "Error: Unknown operand type" << std::endl;
			return "";
		}
		}
	}

	std::string Assembler::assembleOperation(Operation* op)
	{
		return assembleNode(op->lhs) + " " + assembleOperand(op->op) + " " + assembleNode(op->rhs);
	}

	std::string Assembler::assembleUnaryOperation(UnaryOperation* op)
	{
		if (op->side == UnaryOperation::Sided::LEFT)
			return assembleOperand(op->op) + assembleNode(op->val);

		else
			return assembleNode(op->val) + assembleOperand(op->op);
	}

	std::string Assembler::assembleIfStatement(IfStatement* ifStmt)
	{
		std::string out;

		switch (ifStmt->type)
		{
		case IfStatement::IfType::IF:
		{
			out = "if (" + assembleNode(ifStmt->condition) + ")\n{\n";

			for (std::unique_ptr<ASTNode>& node : ifStmt->body)
			{
				out = out + "\t" + assembleNode(node) + "\n";
			}

			out = out + "}\n";

			if (ifStmt->next != nullptr)
			{
				out = out + assembleIfStatement(ifStmt->next.get());
			}

			break;
		}

		case IfStatement::IfType::ELSE_IF:
		{
			out = "else if (" + assembleNode(ifStmt->condition) + ")\n{\n";

			for (std::unique_ptr<ASTNode>& node : ifStmt->body)
			{
				out = out + "\t" + assembleNode(node) + "\n";
			}

			out = out + "}\n";

			if (ifStmt->next != nullptr)
			{
				out = out + assembleIfStatement(ifStmt->next.get());
			}

			break;
		}

		case IfStatement::IfType::ELSE:
		{
			out = "else\n{\n";

			for (std::unique_ptr<ASTNode>& node : ifStmt->body)
			{
				out = out + "\t" + assembleNode(node) + "\n";
			}

			out = out + "}\n";

			if (ifStmt->next != nullptr)
			{
				throw std::runtime_error("How did this even happen?");
			}

			break;
		}
		}

		return out;
	}

	inline std::string Assembler::assembleBracketExpression(BracketedExpression* bracket)
	{
		return "(" + assembleNode(bracket->expr) + ")";
	}

	//

	std::string Assembler::assembleNode(std::unique_ptr<ASTNode>& node)
	{
		// Checks if the node is null
		// IDK how this would happen, but it's better to be safe than sorry
		if (node == nullptr) { return ""; }

		switch (node->type)
		{
			// Casts the node to the correct type and calls the appropriate function
		case ASTNode::NodeType::FUNCTION_CALL: { return assembleFunctionCall(static_cast<FunctionCall*>(node.get())); }
		case ASTNode::NodeType::STRING_LITERAL: { return assembleStringLiteral(static_cast<StringLiteral*>(node.get())); }
		case ASTNode::NodeType::IDENTIFIER: { return assembleIdentifier(static_cast<Identifier*>(node.get())); }
		case ASTNode::NodeType::VARIABLE_DECLARATION: { return assembleVarDec(static_cast<VariableDeclaration*>(node.get())); }
		case ASTNode::NodeType::ASSIGNMENT: { return assembleAssignment(static_cast<Assignment*>(node.get())); }
		case ASTNode::NodeType::OPERATION: { return assembleOperation(static_cast<Operation*>(node.get())); }
		case ASTNode::NodeType::UNARY_OPERATION: { return assembleUnaryOperation(static_cast<UnaryOperation*>(node.get())); }
		case ASTNode::NodeType::IF_STATEMENT: { return assembleIfStatement(static_cast<IfStatement*>(node.get())); }
		case ASTNode::NodeType::BRACKETED_EXPRESSION: { return assembleBracketExpression(static_cast<BracketedExpression*>(node.get())); }
		case ASTNode::NodeType::RETURN_STATEMENT: { return assembleReturnStatement(static_cast<ReturnStatement*>(node.get())); }

												// If the node type is unknown, throw an error with the node type
		default:
		{
			std::cerr << "Error: Unknown node type: " << (int)node->type << "\n" << std::endl;
			return "";
		}
		}
	}

	std::string Assembler::assembleReturnStatement(ReturnStatement* ret)
	{
		return "return " + assembleNode(ret->expr) + ";";
	}

	std::string Assembler::assemble(FileAST& AST)
	{
		// Adds the iostream include (needed for std::cin.get())
		includes["iostream"] = true;

		// Creates the output string with the main function
		std::string out = "";

		// Loops through each node in the AST and assembles it
		for (FunctionDeclaration& functions : AST.functions)
		{
			out = out + assembleVarDecType(&functions.returnTypes[0]) + " " + functions.name.name + "(";

			for (size_t i = 0; i < functions.args.size(); i++)
			{
				if (i != 0) { out = out + ", "; }

				VariableDeclaration* arg = static_cast<VariableDeclaration*>(functions.args[i].get());

				out = out + assembleVarDecType(&arg->varType) + " " + arg->name.name;
			}

			out = out + ")\n{\n";

			for (std::unique_ptr<ASTNode>& node : functions.body)
			{
				out = out + "\t" + assembleNode(node) + "\n";
			}

			out = out + "}\n";
		}

		// Adds the includes to the top of the file
		for (auto& include : includes) { out = "#include <" + include.first + ">\n" + out; }

		return out;
	}
}