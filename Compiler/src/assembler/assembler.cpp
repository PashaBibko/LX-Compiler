#include <assembler/assembler.h>

#include <assembler/lx-core.h>
#include <parser/parser.h>

//

std::string Assembler::assembleFunctionCall(FunctionCall* call)
{
	// Checks if the function is in the core function map
	if (LXCore::funcMap.find(call->funcName.name) != LXCore::funcMap.end())
		return LXCore::funcMap[call->funcName.name](call, *this);

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

std::string Assembler::assembleIdentifier(Identifier* id)
{
	return id->name;
}

std::string Assembler::assembleStringLiteral(StringLiteral* str)
{
	return "\"" + str->value + "\"";
}

std::string Assembler::assembleAssignment(Assignment* assign)
{
	if (assign->val == nullptr)
		return ";";

	return Assembler::assembleIdentifier(&assign->name) + " = " + assembleNode(assign->val) + ";";
}

std::string Assembler::assembleVarMods(VariableDeclaration* var)
{
	std::string out = "";

	if (var->isConst())
		out = out + "const ";

	return out;
}

std::string Assembler::assembleVarDec(VariableDeclaration* var)
{
	if (var->val == nullptr)
		return assembleVarMods(var) + var->varType.name + " " + var->name.name + ";";

	else
		return assembleVarMods(var) + var->varType.name + " " + var->name.name + assembleAssignment(var->val.get());
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
	return "(" + assembleNode(op->lhs) + " " + assembleOperand(op->op) + " " + assembleNode(op->rhs) + ")";
}

std::string Assembler::assembleUnaryOperation(UnaryOperation* op)
{
	if (op->side == UnaryOperation::Sided::LEFT)
		return assembleOperand(op->op) + assembleNode(op->val);

	else
		return assembleNode(op->val) + assembleOperand(op->op);
}

//

std::string Assembler::assembleNode(std::unique_ptr<ASTNode>& node)
{
	if (node == nullptr)
		return "";

	switch (node->type)
	{
		case ASTNode::NodeType::FUNCTION_CALL:
		{
			// Calls the function to assemble the function call with a cast to correct type
			return assembleFunctionCall(static_cast<FunctionCall*>(node.get()));
		}

		case ASTNode::NodeType::STRING_LITERAL:
		{
			// Calls the function to assemble the string literal with a cast to correct type
			return assembleStringLiteral(static_cast<StringLiteral*>(node.get()));
		}

		case ASTNode::NodeType::IDENTIFIER:
		{
			// Calls the function to assemble the identifier with a cast to correct type
			return assembleIdentifier(static_cast<Identifier*>(node.get()));
		}

		case ASTNode::NodeType::VARIABLE_DECLARATION:
		{
			// Calls the function to assemble the variable declaration with a cast to correct type
			return assembleVarDec(static_cast<VariableDeclaration*>(node.get()));
		}

		case ASTNode::NodeType::ASSIGNMENT:
		{
			// Calls the function to assemble the assignment with a cast to correct type
			return assembleAssignment(static_cast<Assignment*>(node.get()));
		}

		case ASTNode::NodeType::OPERATION:
		{
			// Calls the function to assemble the operation with a cast to correct type
			return assembleOperation(static_cast<Operation*>(node.get()));
		}

		case ASTNode::NodeType::UNARY_OPERATION:
		{
			// Calls the function to assemble the unary operation with a cast to correct type
			return assembleUnaryOperation(static_cast<UnaryOperation*>(node.get()));
		}

		default:
		{
			std::cerr << "Error: Unknown node type\n" << std::endl;
			return "";
		}
	}
}

std::string Assembler::assemble(FileAST& AST)
{
	std::string out = "#include <iostream>\n\nint main()\n{\n";

	for (std::unique_ptr<ASTNode>& node : AST.script)
	{
		out = out + "\t" + assembleNode(node);
		out = out + "\n";
	}

	return out + "\n\tstd::cin.get();\n}\n";
}