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

#include <parser.h>

#include <common.h>

namespace LX::Parser
{
	// Namespace to hold all the constexpr functions
	// This is to help split them up from the parser code
	namespace Constexprs
	{
		constexpr bool isOperator(LX::Lexer::FuncToken::Type type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Basic math operators
				case FuncToken::Type::PLUS:
				case FuncToken::Type::MINUS:
				case FuncToken::Type::MULTIPLY:
				case FuncToken::Type::DIVIDE:
				case FuncToken::Type::MODULO:

				// The rest of the operators
				case FuncToken::Type::PLUS_EQUALS:
				case FuncToken::Type::MINUS_EQUALS:
				case FuncToken::Type::MULTIPLY_EQUALS:
				case FuncToken::Type::DIVIDE_EQUALS:

				// Binary comparison operators
				case FuncToken::Type::EQUALS:
				case FuncToken::Type::LESS_THAN:
				case FuncToken::Type::GREATER_THAN:
				case FuncToken::Type::LESS_THAN_EQUALS:
				case FuncToken::Type::GREATER_THAN_EQUALS:
				case FuncToken::Type::NOT_EQUALS:

				// Logial operators
				case FuncToken::Type::AND:
				case FuncToken::Type::NOT:
				case FuncToken::Type::OR:

				// Unary operators
				case FuncToken::Type::INCREMENT:
				case FuncToken::Type::DECREMENT:

				// Returns true if the token is an operator
					return true;

				// Default case
				default:
					return false;
			}
		}

		constexpr bool isUnaryOnlyOperator(LX::Lexer::FuncToken::Type type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Unary operators
			case FuncToken::Type::INCREMENT:
			case FuncToken::Type::DECREMENT:
			case FuncToken::Type::NOT:

				// Returns true if the token is a unary operator
				return true;

				// Default case
			default:
				return false;
			}
		}

		constexpr bool isUnaryOperator(LX::Lexer::FuncToken::Type type)
		{
			using namespace LX::Lexer;

			if (isUnaryOnlyOperator(type))
				return true;

			switch (type)
			{
				// Binary operators
			case FuncToken::Type::PLUS:
			case FuncToken::Type::MINUS:

				// Returns true if the token is a unary operator
				return true;

				// Default case
			default:
				return false;
			}
		}

		constexpr bool isVariableDeclaration(LX::Lexer::FuncToken::Type type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Var Types
			case FuncToken::Type::INT_DEC:
			case FuncToken::Type::STR_DEC:

				// Var Modifiers
			case FuncToken::Type::CONST:

				// Returns true if the token is a variable declaration
				return true;

				// Default case
			default:
				return false;
			}
		}

		constexpr bool isVarModifier(LX::Lexer::FuncToken::Type type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Var Modifiers
			case FuncToken::Type::CONST:

				// Returns true if the token is a variable modifier
				return true;

				// Default case
			default:
				return false;
			}
		}
	}

	std::vector<std::unique_ptr<ASTNode>> Parser::parseBlock()
	{
		// Creates a vector to hold the output
		std::vector<std::unique_ptr<ASTNode>> out;

		// Check for the left brace
		if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::LEFT_BRACE)
		{
			throw std::runtime_error("Expected left brace");
		}

		// Skip the left brace
		currentIndex++;

		// Loop through the body
		while (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_BRACE)
		{
			// Parse the body
			out.push_back(parseIfStatement());
		}

		// Skip the right brace
		currentIndex++;

		return out;
	}

	// The parser is split into a call stack
	// This means the bottom of the stack is the first function called
	// It then calls the next function in the stack (if needed)

	// The call stack is as follows:
	// - parseFunctionDeclaration
	// - parseIfStatement
	// - parseVariableDeclaration
	// - parseAssignment
	// - parseFunctionCall
	// - parseOperation
	// - parsePrimary - Should always be the last function called

	std::unique_ptr<ASTNode> Parser::parsePrimary()
	{
		// Switch statement to handle the different types of primary expressions
		switch (currentTokens->operator[](currentIndex).type)
		{
			case LX::Lexer::FuncToken::Type::STRING_LITERAL:
			{
				// Return a StringLiteral type
				return std::make_unique<StringLiteral>(currentTokens->operator[](currentIndex).value);
			}

			case LX::Lexer::FuncToken::Type::IDENTIFIER:
			{
				// Return an Identifier type
				return std::make_unique<Identifier>(currentTokens->operator[](currentIndex).value);
			}

			case LX::Lexer::FuncToken::Type::LEFT_PAREN:
			{
				// Skip the left parenthesis
				currentIndex++;

				// Create the output as a BracketedExpression type to allow access
				std::unique_ptr<BracketedExpression> out = std::make_unique<BracketedExpression>();

				// Parse the value
				out->expr = parseFunctionCall();

				// Check for the right parenthesis
				if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_PAREN)
				{
					std::cerr << "ERROR: Expected right parenthesis" << std::endl;
					return nullptr;
				}

				// Return the output
				return out;
			}

			default:
			{
				// Throw an error if the token is unknown
				std::cout << "WARNING: Passed default node at " << currentIndex << " of type: " << (int)currentTokens->operator[](currentIndex).type << std::endl;
				return std::make_unique<Identifier>("DEFAULT");
			}
		}
	}

	std::unique_ptr<ASTNode> Parser::parseUnaryOperation()
	{
		// Create the output as an UnaryOperation type to allow access
		std::unique_ptr<UnaryOperation> out = std::make_unique<UnaryOperation>();

		// Set the operator
		out->op = currentTokens->operator[](currentIndex).type;

		// Skip the operator
		currentIndex++;

		// Parse the value
		out->val = parseFunctionCall();

		// Set the side of the operation
		out->side = UnaryOperation::Sided::LEFT;

		// Return the output
		return out;
	}

	std::unique_ptr<ASTNode> Parser::parseOperation()
	{
		// If the token is an operator, parse the unary operation
		if (Constexprs::isUnaryOperator(currentTokens->operator[](currentIndex).type))
		{
			// Calls the unary operation parser
			return parseUnaryOperation();
		}

		// Parses the lhs
		std::unique_ptr<ASTNode> lhs = parsePrimary();

		// Iterates to the next token
		currentIndex++;

		if (Constexprs::isOperator(currentTokens->operator[](currentIndex).type))
		{
			// Create the output as an Operation type to allow access
			std::unique_ptr<Operation> out = std::make_unique<Operation>();

			// Set the operator
			out->op = currentTokens->operator[](currentIndex).type;

			// Skip the operator
			currentIndex++;

			// Check if the operator is unary only
			if (Constexprs::isUnaryOnlyOperator(out->op))
			{
				// Create the output as an UnaryOperation type to allow access
				std::unique_ptr<UnaryOperation> unaryOut = std::make_unique<UnaryOperation>();

				// Moves the values to the unary operation
				unaryOut->val = std::move(lhs);
				unaryOut->op = out->op;

				// Set the side of the operation
				unaryOut->side = UnaryOperation::Sided::RIGHT;

				// Return the output
				return unaryOut;
			}

			else
			{
				// Parse the rhs
				out->rhs = parseFunctionCall();

				// Set the lhs
				out->lhs = std::move(lhs);

				// Return the output
				return out;
			}
		}

		// Return the lhs if there is no operator
		return lhs;
	}

	std::unique_ptr<ASTNode> Parser::parseFunctionCall()
	{
		// Checks if the current tokens are a function call
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::IDENTIFIER && currentTokens->operator[](currentIndex + 1).type == LX::Lexer::FuncToken::Type::LEFT_PAREN)
		{
			// Create the output as a FunctionCall type to allow access
			std::unique_ptr<FunctionCall> out = std::make_unique<FunctionCall>();

			// Set the function name
			out->funcName.name = currentTokens->operator[](currentIndex).value;

			// Skip the function name and the left parenthesis
			currentIndex = currentIndex + 2;

			// Loops through the arguments
			while (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_PAREN)
			{
				// Switch statement to handle the different types of arguments
				switch (currentTokens->operator[](currentIndex).type)
				{
					case LX::Lexer::FuncToken::Type::COMMA:
						// Iterate to skip the comma
						currentIndex++;
						break;

					default:
						// Default case (follow the call chain)
						out->args.push_back(parseFunctionCall());
						break;
				}
			}

			// Skip the right parenthesis
			currentIndex++;

			// Returns the output
			return out;
		}

		// Goes down the call chain
		return parseOperation();
	}

	std::unique_ptr<ASTNode> Parser::parseReturnStatement()
	{
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::RETURN)
		{
			// Skip the return token
			currentIndex++;

			// Create the output as a ReturnStatement type to allow access
			std::unique_ptr<ReturnStatement> out = std::make_unique<ReturnStatement>();

			// Parse the value
			out->expr = parseFunctionCall();

			// Return the output
			return out;

		}

		return parseFunctionCall();
	}

	std::unique_ptr<ASTNode> Parser::parseAssignment()
	{
		// Parses the first token
		std::unique_ptr<ASTNode> asignee = parseReturnStatement();

		if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::ASSIGN)
		{
			// Create the output as an Assignment type to allow access
			std::unique_ptr<Assignment> out = std::make_unique<Assignment>();

			// Checj if the asignee is an identifier
			if (asignee->type != ASTNode::NodeType::IDENTIFIER)
			{
				std::cerr << "ERROR: Assignment to non-identifier" << std::endl;
				return nullptr;
			}

			// Sets the name of the assignment
			out->name.name = dynamic_cast<Identifier*>(asignee.get())->name;

			// Skip the assignment operator
			currentIndex++;

			// Parse the value
			out->val = parseFunctionCall();

			// Iterate to the next token
			currentIndex++;

			// Return the output
			return out;
		}

		// Go down the call chain
		return asignee;
	}

	std::unique_ptr<ASTNode> Parser::parseVariableDeclaration()
	{
		// Checks wether token is var declaration relevant
		if (Constexprs::isVariableDeclaration(currentTokens->operator[](currentIndex).type))
		{
			// Create the output as a VariableDeclaration type to allow access
			std::unique_ptr<VariableDeclaration> out = std::make_unique<VariableDeclaration>();

			// Loops through the variable modifiers
			while (Constexprs::isVarModifier(currentTokens->operator[](currentIndex).type))
			{
				switch (currentTokens->operator[](currentIndex).type)
				{
					case LX::Lexer::FuncToken::Type::CONST:
						out->setConst();
						break;

					default:
						std::cerr << "ERROR: Unknown variable modifier" << std::endl;
						return nullptr;
				}

				// Iterate to the next token
				currentIndex++;
			}

			// Set the type of the variable
			switch (currentTokens->operator[](currentIndex).type)
			{
			case LX::Lexer::FuncToken::Type::INT_DEC:
				out->varType.name = "int";
				break;

			case LX::Lexer::FuncToken::Type::STR_DEC:
				out->varType.name = "string";
				break;
			}

			// Iterate to the next token
			currentIndex++;

			// Set the name of the variable
			out->name.name = currentTokens->operator[](currentIndex).value;

			//
			currentIndex++;

			// 
			if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::ASSIGN)
			{
				// Skip the assignment operator
				currentIndex++;

				// Parse the value
				out->val = std::make_unique<Assignment>();

				out->val->val = parseFunctionCall();
			}

			else
			{
				out->val = nullptr;
			}

			// Return the output
			return out;
		}

		// Go up the call chain
		return parseAssignment();
	}

	std::unique_ptr<ASTNode> Parser::parseIfStatement()
	{
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::IF)
		{
			// Skip the if token
			currentIndex++;

			// Create the output as an IfStatement type to allow access
			std::unique_ptr<IfStatement> out = std::make_unique<IfStatement>(IfStatement::IfType::IF);

			// Check for the left parenthesis
			if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::LEFT_PAREN)
			{
				std::cerr << "ERROR: Expected left parenthesis" << std::endl;
				return nullptr;
			}

			// Skip the left parenthesis
			currentIndex++;

			// Parse the condition
			out->condition = parseFunctionCall();

			// Check for the right parenthesis
			if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_PAREN)
			{
				std::cerr << "ERROR: Expected right parenthesis" << std::endl;
				return nullptr;
			}

			// Skip the right parenthesis
			currentIndex++;

			// Parse the body
			out->body = parseBlock();

			// Creates raw pointer to current if object (will be reassigned in chains)
			IfStatement* currentIf = out.get();

			// Checks for the elif token
			while (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::ELIF)
			{
				// Skip the elif token
				currentIndex++;

				// Create the output as an IfStatement type to allow access
				currentIf->next = std::make_unique<IfStatement>(IfStatement::IfType::ELSE_IF);

				// Check for the left parenthesis
				if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::LEFT_PAREN)
				{
					std::cerr << "ERROR: Expected left parenthesis" << std::endl;
					return nullptr;
				}

				// Skip the left parenthesis
				currentIndex++;

				// Parse the condition
				currentIf->next->condition = parseFunctionCall();

				// Check for the right parenthesis
				if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_PAREN)
				{
					std::cerr << "ERROR: Expected right parenthesis" << std::endl;
					return nullptr;
				}

				// Skip the right parenthesis
				currentIndex++;

				// Parse the body
				currentIf->next->body = parseBlock();

				// Chains the next if statement
				currentIf = currentIf->next.get();
			}

			// Checks for the else token
			if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::ELSE)
			{
				// Skip the else token
				currentIndex++;

				// Create the output as an IfStatement type to allow access
				currentIf->next = std::make_unique<IfStatement>(IfStatement::IfType::ELSE);

				// Parse the body
				currentIf->next->body = parseBlock();
			}

			return out;
		}

		return parseVariableDeclaration();
	}

	FunctionDeclaration Parser::parseFunctionDeclaration()
	{
		if (true) // <- So cursed but its temporary
		{
			// Skip the function token
			currentIndex++;

			// Create the output as a FunctionDeclaration type to allow access
			FunctionDeclaration out;

			// Check for return type
			if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::LEFT_BRACKET)
			{
				currentIndex++;

				switch (currentTokens->operator[](currentIndex).type)
				{
					case LX::Lexer::FuncToken::Type::INT_DEC:
						out.returnTypes.push_back(Identifier("int"));
						break;

					case LX::Lexer::FuncToken::Type::STR_DEC:
						out.returnTypes.push_back(Identifier("std::string"));
						break;

					default:
						break;
				}

				// Check for the closing bracket
				currentIndex++;

				if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_BRACKET)
				{
					std::cerr << "ERROR: Expected closing bracket" << std::endl;
					return FunctionDeclaration();
				}

				currentIndex++;
			}

			if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::IDENTIFIER)
			{
				std::cerr << "ERROR: Expected function name" << std::endl;
				return FunctionDeclaration();
			}

			// Set the name of the function
			out.name.name = currentTokens->operator[](currentIndex).value;

			// Skip the function name
			currentIndex++;

			// Check for the left parenthesis
			if (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::LEFT_PAREN)
			{
				std::cerr << "ERROR: Expected left parenthesis" << std::endl;
				return FunctionDeclaration();
			}

			currentIndex++;

			while (currentTokens->operator[](currentIndex).type != LX::Lexer::FuncToken::Type::RIGHT_PAREN)
			{
				if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::END_OF_SCOPE)
				{
					std::cerr << "ERROR: Expected right parenthesis" << std::endl;
					return FunctionDeclaration();
				}

				out.args.push_back(parseVariableDeclaration());
				if (out.args.back().get()->type != ASTNode::NodeType::VARIABLE_DECLARATION)
				{
					std::cerr << "ERROR: Expected argument" << std::endl;
					return FunctionDeclaration();
				}

				if (currentTokens->operator[](currentIndex).type == LX::Lexer::FuncToken::Type::COMMA)
				{
					currentIndex++;
				}
			}

			currentIndex++;

			out.body = parseBlock();

			return out;
		}

		else
		{
			std::cerr << "ERROR: Expected function declaration: " << (int)currentTokens->operator[](currentIndex).type << std::endl;
			return FunctionDeclaration();
		}
	}

	void Parser::parse(const std::vector<LX::Lexer::FuncToken>& tokens, FileAST& out)
	{
		// Initialize
		currentTokens = &tokens;

		// Throws error if there are no tokens
		if (tokens.size() == 0)
		{
			THROW_ERROR("Token vector is empty");
		}

		// Loop through the tokens
		while ((*currentTokens)[currentIndex].type != LX::Lexer::FuncToken::Type::END_OF_SCOPE)
		{
			out.functions.push_back(parseFunctionDeclaration());
		}
	}
}
