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

#pragma once

#include <std-libs.h>

#include <macro/flag.h>
#include <cdt/token.h>

// Foward declarations
namespace LX::Parser
{
	// Forward declarations
	class ASTNode;

	// Typedefs for this file

	typedef std::vector<std::unique_ptr<ASTNode>> AST;

	/*
	* @brief Base class for all AST nodes
	*/
	class ASTNode
	{
		public:
			// Enum for representing the type of the node
			enum class NodeType : unsigned char
			{
				IDENTIFIER,
				VARIABLE_DECLARATION,
				ASSIGNMENT,
				OPERATION,
				UNARY_OPERATION,
				FUNCTION_CALL,
				STRING_LITERAL,

				BRACKETED_EXPRESSION,

				IF_STATEMENT,

				RETURN_STATEMENT,

				UNDEFINED
			};

			// Constructor
			ASTNode(NodeType type) : type(type) {}

			// Node type for polymorphism
			const NodeType type;

			// Destructor
			virtual ~ASTNode() = default;
	};

	/*
	* @brief Represents an identifier in the AST
	* This is used for variable names, function names, number literals, string literals, etc.
	*/
	class Identifier : public ASTNode
	{
		public:
			// Constructor
			Identifier(std::string name = "") : ASTNode(NodeType::IDENTIFIER), name(name) {}

			// Contents
			std::string name;
	};
	/*
	*/
	class Assignment : public ASTNode
	{
		public:
			// Constructor
			Assignment() : ASTNode(NodeType::ASSIGNMENT) {}

			// Contents
			Identifier name;
			std::unique_ptr<ASTNode> val;
	};

	/*
	* @brief Represents a variable declaration in the AST
	*/
	class VariableDeclaration : public ASTNode
	{
		private:
			Flags flags = 0;

		public:
			// Constructor
			VariableDeclaration() : ASTNode(NodeType::VARIABLE_DECLARATION) {}

			// Contents
			Identifier varType;
			Identifier name;

			std::unique_ptr<Assignment> val;

			// Flags
			FLAG_DEF(Const, 0x01);
			FLAG_DEF(Reference, 0x02);
			FLAG_DEF(Pointer, 0x04);
			FLAG_DEF(Static, 0x08);
			FLAG_DEF(GuideChild, 0x10);
			FLAG_DEF(Unsigned, 0x20);
	};

	/*
	* @brief Represents an mathematic or logical operation in the AST
	*/
	class Operation : public ASTNode
	{
		public:
			// Constructor
			Operation() : ASTNode(NodeType::OPERATION) {}

			// Contents of the operation
			std::unique_ptr<ASTNode> lhs;
			std::unique_ptr<ASTNode> rhs;

			LX::Lexer::TokenType op;
	};

	class UnaryOperation : public ASTNode
	{
		public:
			// Enum for representing the side of the operation
			enum class Sided : bool
			{
				LEFT,
				RIGHT
			};

			// Constructor
			UnaryOperation() : ASTNode(NodeType::UNARY_OPERATION) {}

			// Contents of the operation
			std::unique_ptr<ASTNode> val;

			LX::Lexer::TokenType op;
			Sided side;
	};

	/*
	*/
	class FunctionCall : public ASTNode
	{
		private:
			Flags flags = 0;

		public:
			// Constructor
			FunctionCall() : ASTNode(NodeType::FUNCTION_CALL) {}

			// Name
			Identifier funcName;

			// Arguments
			std::vector<std::unique_ptr<ASTNode>> args;
	};

	/*
	*/
	class StringLiteral : public ASTNode
	{
		public:
			// Constructor
			StringLiteral(std::string value) : ASTNode(NodeType::STRING_LITERAL), value(value) {}

			// Contents
			std::string value;
	};

	class IfStatement : public ASTNode
	{
		public:
			//
			enum class IfType : char
			{
				IF,
				ELSE_IF,
				ELSE
			};

			// Constructor
			IfStatement(IfType t) : ASTNode(NodeType::IF_STATEMENT), type(t) {}

			// Condition
			std::unique_ptr<ASTNode> condition;

			// Body
			std::vector<std::unique_ptr<ASTNode>> body;

			// Type
			IfType type;

			// Chain
			std::unique_ptr<IfStatement> next;
	};

	class BracketedExpression : public ASTNode
	{
		public:
			// Constructor
			BracketedExpression() : ASTNode(NodeType::BRACKETED_EXPRESSION) {}

			// Contents
			std::unique_ptr<ASTNode> expr;
	};

	class ReturnStatement : public ASTNode
	{
		public:
			// Constructor
			ReturnStatement() : ASTNode(NodeType::RETURN_STATEMENT) {}

			// Contents
			std::unique_ptr<ASTNode> expr;
	};

	class FunctionDeclaration
	{
		public:
			// Constructor
			FunctionDeclaration() {}

			// Name
			Identifier name;

			// Return type
			std::vector<Identifier> returnTypes;

			// Arguments
			std::vector<std::unique_ptr<ASTNode>> args;

			// Body
			AST body;
	};

	/*
	* @brief Wrapper class for the AST of a file
	*
	* @note This will eventually store items such as:
	* - Function declarations
	* - Class declarations
	*/
	struct FileAST
	{
		std::vector<FunctionDeclaration> functions;
	};
};
