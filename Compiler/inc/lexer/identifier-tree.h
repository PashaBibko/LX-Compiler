#pragma once

#include <lexer/token.h>

#include <error.h>

#include <unordered_map>
#include <memory>

namespace lx
{
	class Node
	{
		public:
			// Enum to classify the type of the node
			enum class NodeType
			{
				// Callable types
				FUNCTION,
				SHADER,

				// Creatable types
				STRUCT,
				CLASS,
				MACRO,
				ENUM,

				// Non-callable types
				VARIABLE,
				CONSTANT, // Not a constant variable, but a constant value (constexpr or #define)

				// Container types
				SCOPE, // Scopes do not have names
				NAMESPACE,

				// Undefined type
				UNDEFINED = -1
			};

			// Adds a child to the node
			// Returns a pointer to the created child
			// Will return a nullptr if the child already exists (scopes will always be created)
			Node* createChild(NodeType type, std::string name);

			// Checks if a child with the given name exists
			NodeType getChildType(std::string name, bool calledViaParent); 

			// Static instance of the class 
			// Called the globalScope or identifier tree
			static Node globalScope;

		private:
			// Counter for the ammount of scopes created within this scope
			// Used to create unique names for scopes
			unsigned int scopeCounter = 0;

			// Pointer to the parent of this node
			// Will be nullptr if the node is the global scope
			Node* parent = nullptr;

			// Map of all the children of this node
			// Only is not null if the node is a scope or namespace
			std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<Node>>> children;

			// The type of the node
			NodeType type = NodeType::UNDEFINED;

			// Constructor that takes the type of the node
			// Private to make sure that the createChild function is used so new nodes are assigned to the global scope
			Node(NodeType type) : type(type) {};
	};
}