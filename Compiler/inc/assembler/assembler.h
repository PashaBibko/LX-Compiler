#pragma once

#include <assembler/lx-core.h>
#include <parser/parser.h>

class Assembler
{
	private:
		friend std::string assembleNode(std::unique_ptr<ASTNode>& node, Assembler& assembler);

		std::string assembleFunctionCall(FunctionCall* call)
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

				output = output + assembleNode(arg, *this);

				firstArg = false;
			}

			// Adds the closing bracket with the return statement
			return output + ");";
		}

	public:
		Assembler() = default;

		std::string assemble(FileAST& AST)
		{
			std::string out = "#include <iostream>\n\nint main()\n{\n";

			for (std::unique_ptr<ASTNode>& node : AST.script)
			{
				out = out + assembleNode(node, *this);
				out = out + "\n";
			}

			return out + "\nstd::cin.get();\n}\n";
		}
};

std::string assembleIdentifier(Identifier* id, Assembler& assembler)
{
	return id->name;
}

std::string assembleStringLiteral(StringLiteral* str, Assembler& assembler)
{
	return "\"" + str->value + "\"";
}

std::string assembleAssignment(Assignment* assign, Assembler& assembler, bool passedFromVarDec = false)
{
	if (assign->val == nullptr)
		return ";";

	return assembleIdentifier(&assign->name, assembler) + " = " + assembleNode(assign->val, assembler) + ";";
}

std::string assembleVarDec(VariableDeclaration* var, Assembler& assembler)
{
	if (var->val == nullptr)
		return var->varType.name + " " + var->name.name + ";";

	else
		return var->varType.name + " " + var->name.name + assembleAssignment(var->val.get(), assembler, true);
}

std::string assembleNode(std::unique_ptr<ASTNode>& node, Assembler& assembler)
{
	switch (node->type)
	{
		case ASTNode::NodeType::FUNCTION_CALL:
		{
			// Calls the function to assemble the function call with a cast to correct type
			return assembler.assembleFunctionCall(static_cast<FunctionCall*>(node.get()));
		}

		case ASTNode::NodeType::STRING_LITERAL:
		{
			// Calls the function to assemble the string literal with a cast to correct type
			return assembleStringLiteral(static_cast<StringLiteral*>(node.get()), assembler);
		}

		case ASTNode::NodeType::IDENTIFIER:
		{
			// Calls the function to assemble the identifier with a cast to correct type
			return assembleIdentifier(static_cast<Identifier*>(node.get()), assembler);
		}

		case ASTNode::NodeType::VARIABLE_DECLARATION:
		{
			// Calls the function to assemble the variable declaration with a cast to correct type
			return assembleVarDec(static_cast<VariableDeclaration*>(node.get()), assembler);
		}

		case ASTNode::NodeType::ASSIGNMENT:
		{
			// Calls the function to assemble the assignment with a cast to correct type
			return assembleAssignment(static_cast<Assignment*>(node.get()), assembler);
		}

		default:
		{
			std::cerr << "Error: Unknown node type" << std::endl;
			return "";
		}
	}
}
