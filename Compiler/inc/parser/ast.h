#pragma once

#include <lexer/token.h>

#include <iostream>
#include <memory>
#include <variant>
#include <vector>
#include <tuple>

// Foward declarations
namespace lx
{

	class ASTNode;

	// Typedefs for this file

	typedef unsigned char Flags;
	typedef std::vector<std::unique_ptr<ASTNode>> AST;

#define FLAG_VAL(name, value) static constexpr Flags name = value;
#define FLAG_SET(name) inline void set##name() { flags |= name##FlagVal; }
#define FLAG_GET(name) inline bool is##name() const { return flags & name##FlagVal; }

#define FLAG_DEF(name, value) FLAG_VAL(name##FlagVal, value) FLAG_SET(name) FLAG_GET(name)

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

		TokenType op;
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

		TokenType op;
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
		// Body
		std::vector<std::unique_ptr<ASTNode>> body;
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
	// Debug function
	void displayAST(std::unique_ptr<ASTNode>& node, short depth = 0);

	/*
	* @brief Wrapper class for the AST of a file
	*
	* @note This will eventually store items such as:
	* - Function declarations
	* - Class declarations
	*/
	struct FileAST
	{
		AST script;

		void display()
		{
			for (auto& node : script)
			{
				displayAST(node);
				std::cout << std::endl;
			}
		}
	};

}
