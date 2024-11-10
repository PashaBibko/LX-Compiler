// ======================================================================================= //
//                                                                                         //
// This code is license under a Proprietary License for LX - Compiler                      //
//                                                                                         //
// Copyright(c) 2024 Pasha Bibko                                                           //
//                                                                                         //
// 1. License Grant                                                                        //
//     You are granted a non - exclusive, non - transferable, and revocable                //
//     license to use this software for personal, educational, non - commercial,           //
//     or internal commercial purposes.You may install and use the software on             //
//     your devices or within your company, but you may not sell, sublicense,              //
//     or distribute the software in any form, either directly or as part                  //
//     of any derivative works. You may privately modify the software for                  //
//     internal use within your organization, provided that the modified versions          //
//     are not distributed, shared, or otherwise made available to third parties.          //
//                                                                                         //
// 2. Freedom to Share Creations                                                           //
//     You are free to create, modify, and share works or creations made with this         //
//     software, provided that you do not redistribute the original software itself.       //
//     All creations made with this software are solely your responsibility, and           //
//     you may license or distribute them as you wish, under your own terms.               //
//                                                                                         //
// 3. Restrictions                                                                         //
//     You may not:                                                                        //
//     - Sell, rent, lease, or distribute the original software or any copies              //
//       thereof, including modified versions.                                             //
//     - Distribute the software or modified versions to any third party.                  //
//                                                                                         //
// 4. Disclaimer of Warranty                                                               //
//     This software is provided "as is", without warranty of any kind, either             //
//     express or implied, including but not limited to the warranties of merchantability, //
//     fitness for a particular purpose, or non - infringement.In no event shall the       //
//     authors or copyright holders be liable for any claim, damages, or other liability,  //
//     whether in an action of contract, tort, or otherwise, arising from, out of, or in   //
//     connection with the software or the use or other dealings in the software.          //
//                                                                                         //
// ======================================================================================= //

#include <translator.h>

#include <common.h>

#include <lx-core.h>

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