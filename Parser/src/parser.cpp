#include <parser.h>

#include <cdt/ast.h>

#include <debug/error.h>

#include <macro/dll-func.h>

#include <debug/DebugLog.h>

namespace LX::Parser
{
	// Namespace to hold all the constexpr functions
	// This is to help split them up from the parser code
	namespace Constexprs
	{
		constexpr bool isOperator(LX::Lexer::TokenType type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Basic math operators
				case TokenType::PLUS:
				case TokenType::MINUS:
				case TokenType::MULTIPLY:
				case TokenType::DIVIDE:
				case TokenType::MODULO:

				// The rest of the operators
				case TokenType::PLUS_EQUALS:
				case TokenType::MINUS_EQUALS:
				case TokenType::MULTIPLY_EQUALS:
				case TokenType::DIVIDE_EQUALS:

				// Binary comparison operators
				case TokenType::EQUALS:
				case TokenType::LESS_THAN:
				case TokenType::GREATER_THAN:
				case TokenType::LESS_THAN_EQUALS:
				case TokenType::GREATER_THAN_EQUALS:
				case TokenType::NOT_EQUALS:

				// Logial operators
				case TokenType::AND:
				case TokenType::NOT:
				case TokenType::OR:

				// Unary operators
				case TokenType::INCREMENT:
				case TokenType::DECREMENT:

				// Returns true if the token is an operator
					return true;

				// Default case
				default:
					return false;
			}
		}

		constexpr bool isUnaryOnlyOperator(LX::Lexer::TokenType type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Unary operators
			case TokenType::INCREMENT:
			case TokenType::DECREMENT:
			case TokenType::NOT:

				// Returns true if the token is a unary operator
				return true;

				// Default case
			default:
				return false;
			}
		}

		constexpr bool isUnaryOperator(LX::Lexer::TokenType type)
		{
			using namespace LX::Lexer;

			if (isUnaryOnlyOperator(type))
				return true;

			switch (type)
			{
				// Binary operators
			case TokenType::PLUS:
			case TokenType::MINUS:

				// Returns true if the token is a unary operator
				return true;

				// Default case
			default:
				return false;
			}
		}

		constexpr bool isVariableDeclaration(LX::Lexer::TokenType type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Var Types
			case TokenType::INT_DEC:
			case TokenType::STR_DEC:

				// Var Modifiers
			case TokenType::CONST:

				// Returns true if the token is a variable declaration
				return true;

				// Default case
			default:
				return false;
			}
		}

		constexpr bool isVarModifier(LX::Lexer::TokenType type)
		{
			using namespace LX::Lexer;

			switch (type)
			{
				// Var Modifiers
			case TokenType::CONST:

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
		if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::LEFT_BRACE)
		{
			throw std::runtime_error("Expected left brace");
		}

		// Skip the left brace
		currentIndex++;

		// Loop through the body
		while (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_BRACE)
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
			case LX::Lexer::TokenType::STRING_LITERAL:
			{
				// Return a StringLiteral type
				return std::make_unique<StringLiteral>(currentTokens->operator[](currentIndex).value);
			}

			case LX::Lexer::TokenType::IDENTIFIER:
			{
				// Return an Identifier type
				return std::make_unique<Identifier>(currentTokens->operator[](currentIndex).value);
			}

			case LX::Lexer::TokenType::LEFT_PAREN:
			{
				// Skip the left parenthesis
				currentIndex++;

				// Create the output as a BracketedExpression type to allow access
				std::unique_ptr<BracketedExpression> out = std::make_unique<BracketedExpression>();

				// Parse the value
				out->expr = parseFunctionCall();

				// Check for the right parenthesis
				if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_PAREN)
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
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::IDENTIFIER && currentTokens->operator[](currentIndex + 1).type == LX::Lexer::TokenType::LEFT_PAREN)
		{
			// Create the output as a FunctionCall type to allow access
			std::unique_ptr<FunctionCall> out = std::make_unique<FunctionCall>();

			// Set the function name
			out->funcName.name = currentTokens->operator[](currentIndex).value;

			// Skip the function name and the left parenthesis
			currentIndex = currentIndex + 2;

			// Loops through the arguments
			while (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_PAREN)
			{
				// Switch statement to handle the different types of arguments
				switch (currentTokens->operator[](currentIndex).type)
				{
					case LX::Lexer::TokenType::COMMA:
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
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::RETURN)
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

		if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::ASSIGN)
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
					case LX::Lexer::TokenType::CONST:
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
			case LX::Lexer::TokenType::INT_DEC:
				out->varType.name = "int";
				break;

			case LX::Lexer::TokenType::STR_DEC:
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
			if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::ASSIGN)
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
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::IF)
		{
			// Skip the if token
			currentIndex++;

			// Create the output as an IfStatement type to allow access
			std::unique_ptr<IfStatement> out = std::make_unique<IfStatement>(IfStatement::IfType::IF);

			// Check for the left parenthesis
			if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::LEFT_PAREN)
			{
				std::cerr << "ERROR: Expected left parenthesis" << std::endl;
				return nullptr;
			}

			// Skip the left parenthesis
			currentIndex++;

			// Parse the condition
			out->condition = parseFunctionCall();

			// Check for the right parenthesis
			if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_PAREN)
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
			while (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::ELIF)
			{
				// Skip the elif token
				currentIndex++;

				// Create the output as an IfStatement type to allow access
				currentIf->next = std::make_unique<IfStatement>(IfStatement::IfType::ELSE_IF);

				// Check for the left parenthesis
				if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::LEFT_PAREN)
				{
					std::cerr << "ERROR: Expected left parenthesis" << std::endl;
					return nullptr;
				}

				// Skip the left parenthesis
				currentIndex++;

				// Parse the condition
				currentIf->next->condition = parseFunctionCall();

				// Check for the right parenthesis
				if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_PAREN)
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
			if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::ELSE)
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
		if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::FUNCTION)
		{
			// Skip the function token
			currentIndex++;

			// Create the output as a FunctionDeclaration type to allow access
			FunctionDeclaration out;

			// Check for return type
			if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::LEFT_BRACKET)
			{
				currentIndex++;

				switch (currentTokens->operator[](currentIndex).type)
				{
					case LX::Lexer::TokenType::INT_DEC:
						out.returnTypes.push_back(Identifier("int"));
						break;

					case LX::Lexer::TokenType::STR_DEC:
						out.returnTypes.push_back(Identifier("std::string"));
						break;

					default:
						break;
				}

				// Check for the closing bracket
				currentIndex++;

				if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_BRACKET)
				{
					std::cerr << "ERROR: Expected closing bracket" << std::endl;
					return FunctionDeclaration();
				}

				currentIndex++;
			}

			if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::IDENTIFIER)
			{
				std::cerr << "ERROR: Expected function name" << std::endl;
				return FunctionDeclaration();
			}

			// Set the name of the function
			out.name.name = currentTokens->operator[](currentIndex).value;

			// Skip the function name
			currentIndex++;

			// Check for the left parenthesis
			if (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::LEFT_PAREN)
			{
				std::cerr << "ERROR: Expected left parenthesis" << std::endl;
				return FunctionDeclaration();
			}

			currentIndex++;

			while (currentTokens->operator[](currentIndex).type != LX::Lexer::TokenType::RIGHT_PAREN)
			{
				if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::END_OF_FILE)
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

				if (currentTokens->operator[](currentIndex).type == LX::Lexer::TokenType::COMMA)
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

	void Parser::parse(const std::vector<LX::Lexer::Token>& tokens, FileAST& out, bool debugMode)
	{
		// Initialize
		currentTokens = &tokens;

		// Throws error if there are no tokens
		if (tokens.size() == 0)
		{
			THROW_ERROR("Token vector is empty");
		}

		// Loop through the tokens
		while ((*currentTokens)[currentIndex].type != LX::Lexer::TokenType::END_OF_FILE)
		{
			out.functions.push_back(parseFunctionDeclaration());

			LX::Debug::DebugLog(&out.functions.back(), 0);
		}
	}
}
