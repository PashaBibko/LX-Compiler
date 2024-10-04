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

std::string Assembler::assembleVarDec(VariableDeclaration* var)
{
	if (var->val == nullptr)
		return var->varType.name + " " + var->name.name + ";";

	else
		return var->varType.name + " " + var->name.name + assembleAssignment(var->val.get());
}

std::string Assembler::assembleOperand(TokenType op)
{
	switch (op)
	{
		case TokenType::PLUS:
		{
			return "+";
		}

		case TokenType::MINUS:
		{
			return "-";
		}

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

//

std::string Assembler::assembleNode(std::unique_ptr<ASTNode>& node)
{
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

		default:
		{
			std::cerr << "Error: Unknown node type" << std::endl;
			return "";
		}
	}
}

std::string Assembler::assemble(FileAST& AST)
{
	std::string out = "#include <iostream>\n\nint main()\n{\n";

	for (std::unique_ptr<ASTNode>& node : AST.script)
	{
		out = out + assembleNode(node);
		out = out + "\n";
	}

	return out + "\nstd::cin.get();\n}\n";
}