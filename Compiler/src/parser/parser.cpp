#include <parser/parser.h>

#include <parser/ast.h>


// Namespace to hold all the constexpr functions
// This is to help split them up from the parser code
namespace Constexprs
{
	constexpr bool isOperator(TokenType type)
	{
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

			// Power operator
			case TokenType::POWER:

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

			// Returns true if the token is an operator
				return true;

			// Default case
			default:
				return false;
		}
	}

	constexpr bool isVariableDeclaration(TokenType type)
	{
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

	constexpr bool isVarModifier(TokenType type)
	{
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

// The parser is split into a call stack
// This means the bottom of the stack is the first function called
// It then calls the next function in the stack (if needed)

// The call stack is as follows:
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
		case TokenType::STRING_LITERAL:
		{
			// Return a StringLiteral type
			return std::make_unique<StringLiteral>(currentTokens->operator[](currentIndex).value);
		}

		case TokenType::IDENTIFIER:
		{
			// Return an Identifier type
			return std::make_unique<Identifier>(currentTokens->operator[](currentIndex).value);
		}

		case TokenType::LEFT_PAREN:
		{
			// Skip the left parenthesis
			currentIndex++;

			// Parse the operation
			std::unique_ptr<ASTNode> out = parseOperation();

			// Return the output
			return out;
		}

		default:
		{
			// Throw an error if the token is unknown
			std::cout << "WARNING: nullptr thrown at " << currentIndex << std::endl;
			std::cout << (int)currentTokens->operator[](currentIndex).type << std::endl;
			return std::make_unique<ASTNode>(ASTNode::NodeType::UNDEFINED);
		}
	}
}

std::unique_ptr<ASTNode> Parser::parseOperation()
{
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

		// Parse the rhs
		out->rhs = parseFunctionCall();

		// Set the lhs
		out->lhs = std::move(lhs);

		// Return the output
		return out;
	}

	// Return the lhs if there is no operator
	return lhs;
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall()
{
	// Checks if the current tokens are a function call
	if (currentTokens->operator[](currentIndex).type == TokenType::IDENTIFIER && currentTokens->operator[](currentIndex + 1).type == TokenType::LEFT_PAREN)
	{
		// Create the output as a FunctionCall type to allow access
		std::unique_ptr<FunctionCall> out = std::make_unique<FunctionCall>();

		// Set the function name
		out->funcName.name = currentTokens->operator[](currentIndex).value;

		// Skip the function name and the left parenthesis
		currentIndex = currentIndex + 2;

		// Loops through the arguments
		while (currentTokens->operator[](currentIndex).type != TokenType::RIGHT_PAREN)
		{
			// Switch statement to handle the different types of arguments
			switch (currentTokens->operator[](currentIndex).type)
			{
				case TokenType::COMMA:
					// Iterate to skip the comma
					currentIndex++;
					break;

				case TokenType::STRING_LITERAL:
					// Add the string literal to the arguments (and iterate over it)
					out->args.push_back(std::make_unique<StringLiteral>(currentTokens->operator[](currentIndex).value));
					currentIndex++;
					break;

				case TokenType::IDENTIFIER:
					// Add the identifier to the arguments (and iterate over it)
					out->args.push_back(std::make_unique<Identifier>(currentTokens->operator[](currentIndex).value));
					currentIndex++;
					break;

				default:
					// Default case (follow the call chain)
					out->args.push_back(parseFunctionCall());
					currentIndex++;
					break;
			}
		}

		// Skip the right parenthesis
		currentIndex++;

		// Returns the output
		return std::move(out);
	}

	// Goes down the call chain
	return parseOperation();
}

std::unique_ptr<ASTNode> Parser::parseAssignment()
{
	// Parses the first token
	std::unique_ptr<ASTNode> asignee = parseFunctionCall();

	if (currentTokens->operator[](currentIndex).type == TokenType::ASSIGN)
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
			case TokenType::CONST:
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
		case TokenType::INT_DEC:
			out->varType.name = "int";
			break;

		case TokenType::STR_DEC:
			out->varType.name = "str";
			break;
		}

		// Iterate to the next token
		currentIndex++;

		// Set the name of the variable
		out->name.name = currentTokens->operator[](currentIndex).value;

		//
		currentIndex++;

		// 
		if (currentTokens->operator[](currentIndex).type == TokenType::ASSIGN)
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

void Parser::parse(const std::vector<Token>& tokens, FileAST& out)
{
	// Initialize
	currentTokens = &tokens;

	// Loop through the tokens
	while (currentTokens->operator[](currentIndex).type != TokenType::END_OF_FILE)
	{
		out.script.push_back(parseVariableDeclaration());

		if (out.script.back() == nullptr)
		{
			std::cerr << "Early Parser Return" << std::endl;
			return;
		}
	}
}
