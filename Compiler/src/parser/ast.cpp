#include <parser/ast.h>

#include <lexer/token.h>

#include <iostream>
#include <memory>
#include <variant>
#include <vector>
#include <tuple>

namespace lx
{
	void displayAST(std::unique_ptr<ASTNode>& node, short depth)
	{
		std::string depthStr = std::string(depth, '\t');

		if (node == nullptr)
		{
			std::cout << depthStr << "NULL NODE" << std::endl;
			return;
		}

		switch (node->type)
		{
		case ASTNode::NodeType::IDENTIFIER:
		{
			Identifier* identifier = dynamic_cast<Identifier*>(node.get());
			std::cout << depthStr << "IDENTIFIER : { " << identifier->name << " }\n";
			break;
		}

		case ASTNode::NodeType::VARIABLE_DECLARATION:
		{
			VariableDeclaration* varDecl = dynamic_cast<VariableDeclaration*>(node.get());

			std::cout << depthStr << "VARIABLE_DECLARATION : " << varDecl->name.name << std::endl;
			std::cout << depthStr << "\tType: " << varDecl->varType.name << std::endl;

			std::cout << depthStr << "\tFlags: " << std::endl;
			std::cout << depthStr << "\t\tConst: " << varDecl->isConst() << std::endl;
			std::cout << depthStr << "\t\tReference: " << varDecl->isReference() << std::endl;
			std::cout << depthStr << "\t\tPointer: " << varDecl->isPointer() << std::endl;
			std::cout << depthStr << "\t\tStatic: " << varDecl->isStatic() << std::endl;
			std::cout << depthStr << "\t\tGuideChild: " << varDecl->isGuideChild() << std::endl;
			std::cout << depthStr << "\t\tUnsigned: " << varDecl->isUnsigned() << std::endl;

			if (varDecl->val != nullptr)
			{
				std::cout << "\tValue:\n";
				displayAST(varDecl->val->val, depth + 2);
			}

			break;
		}

		case ASTNode::NodeType::OPERATION:
		{
			Operation* operation = dynamic_cast<Operation*>(node.get());
			std::cout << depthStr << "OPERATION:" << std::endl;

			displayAST(operation->lhs, depth + 1);
			std::cout << depthStr << "\tOPERATOR : " << (short)operation->op << std::endl;
			displayAST(operation->rhs, depth + 1);

			break;
		}

		case ASTNode::NodeType::FUNCTION_CALL:
		{
			FunctionCall* funcCall = dynamic_cast<FunctionCall*>(node.get());
			std::cout << depthStr << "FUNCTION_CALL :\n";
			std::cout << depthStr << "\tName: " << funcCall->funcName.name << std::endl;

			if (funcCall->args.size() != 0)
			{
				std::cout << depthStr << "\tArguments (" << funcCall->args.size() << "):" << std::endl;

				for (auto& arg : funcCall->args)
				{
					displayAST(arg, depth + 2);
				}
			}

			std::cout << "\n";

			break;
		}

		case ASTNode::NodeType::STRING_LITERAL:
		{
			StringLiteral* stringLiteral = dynamic_cast<StringLiteral*>(node.get());
			std::cout << depthStr << "STRING_LITERAL : { " << stringLiteral->value << " }" << std::endl;
			break;
		}

		case ASTNode::NodeType::ASSIGNMENT:
		{
			Assignment* assignment = dynamic_cast<Assignment*>(node.get());
			std::cout << depthStr << "ASSIGNMENT : " << assignment->name.name << std::endl;
			displayAST(assignment->val, depth + 1);
			break;
		}

		default:
		{
			std::cout << depthStr << "UNKNOWN NODE TYPE" << std::endl;
			break;
		}
		}
	}
}
