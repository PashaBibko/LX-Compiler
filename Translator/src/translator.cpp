#include <translator.h>

#include <lx-core.h>
#include <cdt/ast.h>

#include <sstream>
#include <fstream>
#include <set>

namespace LX::Translator
{
	std::vector<std::string> Translator::funcList;
	std::vector<std::string> Translator::funcHeaders;

	void Translator::assembleNode(LX::Parser::ASTNode* node)
	{
		nodeTranslators[node->type](*this, node);
	}

	void Translator::assemble(LX::Parser::FunctionDeclaration& AST, const std::string outputDir, const std::string lx_fileName)
	{
		// Adds the function to the function list
		std::string funcDecl = AST.returnTypes[0].name + " " + AST.name.name + "(";

		for (int i = 0; i < AST.args.size(); i++)
		{
			if (i != 0) { funcDecl += ", "; }

			LX::Parser::VariableDeclaration* arg = static_cast<LX::Parser::VariableDeclaration*>(AST.args[i].get());

			if (arg->isConst()) { funcDecl += "const "; }

			if (arg->varType.name == "string")
			{
				includes.insert("string");
				funcDecl += "std::string ";
			}

			else
			{
				funcDecl += arg->varType.name + " ";
			}

			funcDecl += arg->name.name;
		}

		funcDecl += ")";

		// Adds the function declaration to the header list
		funcHeaders.push_back(funcDecl);

		// Adds the function declaration to the output stream
		out << funcDecl << "\n{\n";

		for (std::unique_ptr<LX::Parser::ASTNode>& node : AST.body)
		{
			assembleNode(node.get());
		}

		out << "}\n";
		
		// Creates the function .cpp filename
		std::string fileName = outputDir + "/" + lx_fileName + "_" + AST.name.name + "-f" + ".cpp";

		// Check if the file already exists within the function list (meaning it is an overloaded function)
		if (std::find(funcList.begin(), funcList.end(), fileName) != funcList.end())
		{
			// If it is, add a number to the end of the file name
			int i = 1;
			while (std::find(funcList.begin(), funcList.end(), fileName) != funcList.end())
			{
				fileName = outputDir + "/" + lx_fileName + "_" + AST.name.name + "-f" + std::to_string(i) + ".cpp";
				i++;
			}
		}

		// Adds the file to the function list
		funcList.push_back(fileName);

		// Combines the includes
		std::ostringstream includeStream;
		for (const std::string& include : includes) { includeStream << "#include <" << include << ">\n"; }
		includeStream << "#include <functions.h>\n\n";

		// Writes to the file
		std::ofstream file(fileName);

		file << includeStream.str();
		file << out.str();
	}
}

#include <translate-ast.h>

namespace LX::Translator
{
	std::unordered_map<LX::Parser::ASTNode::NodeType, std::function<void(Translator&, LX::Parser::ASTNode*)>> Translator::nodeTranslators =
	{
		{ LX::Parser::ASTNode::NodeType::IDENTIFIER,					 assembleIdentifier					},
		{ LX::Parser::ASTNode::NodeType::VARIABLE_DECLARATION,			 assembleVariableDeclaration		},
		{ LX::Parser::ASTNode::NodeType::ASSIGNMENT,					 assembleAssignment					},
		{ LX::Parser::ASTNode::NodeType::OPERATION,						 assembleOperation					},
		{ LX::Parser::ASTNode::NodeType::UNARY_OPERATION,				 assembleUnaryOperation				},
		{ LX::Parser::ASTNode::NodeType::FUNCTION_CALL,					 assembleFunctionCall				},
		{ LX::Parser::ASTNode::NodeType::STRING_LITERAL,				 assembleStringLiteral				},
		{ LX::Parser::ASTNode::NodeType::BRACKETED_EXPRESSION,			 assembleBracketedExpression		},
		{ LX::Parser::ASTNode::NodeType::IF_STATEMENT,					 assembleIfStatement				},
		{ LX::Parser::ASTNode::NodeType::RETURN_STATEMENT,				 assembleReturnStatement			},
		{ LX::Parser::ASTNode::NodeType::UNDEFINED,						 assembleUndefined					}
	};
}