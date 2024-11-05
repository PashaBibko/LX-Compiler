#include <lexer/identifier-tree.h>

#include <lexer/token.h>

#include <error.h>

#include <unordered_map>
#include <memory>

namespace lx
{
	Node Node::globalScope = Node(Node::NodeType::SCOPE);
	
	Node::NodeType Node::getChildType(std::string name, bool calledViaParent = false)
	{
		// Returns false if the children map is empty
		if (children == nullptr)
		{
			return NodeType::UNDEFINED;
		}

		// Checks the children map for the name
		if (auto child = children->find(name); child != children->end())
		{
			return child->second->type;
		}

		// Checks if the function was called via the parent node
		if (calledViaParent == true)
		{
			// Checks if the name exists within the children map
			if (auto child = children->find(name); child != children->end())
			{
				return child->second->type;
			}
		}

		else
		{
			// Check recursively if the name exists within the children map
			for (const auto& child : *children)
			{
				if (child.second->getChildType(name, true) != NodeType::UNDEFINED)
				{
					return child.second->getChildType(name, true);
				}
			}
		}

		// Else checks the parent node (recursively)
		if (parent != nullptr)
		{
			if (NodeType type = parent->getChildType(name, true); type != NodeType::UNDEFINED)
			{
				return type;
			}
		}

		// Else the child does not exist
		return NodeType::UNDEFINED;
	}

	Node* Node::createChild(NodeType type, std::string name = "DEFAULT")
	{
		// Checks if name is the default name
		if (name == "DEFAULT" && type != NodeType::SCOPE)
		{
			THROW_ERROR("Name was not provided for a non-scope node");
		}

		// Checks if the name is the default name for a scope
		else if (type == NodeType::SCOPE)
		{
			// Creates a scope name
			name = "s_" + std::to_string(scopeCounter);
			scopeCounter++;
		}

		else
		{
			// Adds a prefix to stop errors from happening when users start identifiers with: {s_}
			name = "_" + name;
		}

		// Checks that the given type is not UNDEFINED
		if (type == NodeType::UNDEFINED)
		{
			THROW_ERROR("Cannot create node of type UNDEFINED");
		}

		// Checks if the name exists within the current scope
		if (getChildType(name) != NodeType::UNDEFINED)
		{
			return nullptr;
		}

		// Else adds it to the children map
		children->insert({ name, std::make_unique<Node>(type) });

		// Gets the pointer to the created child
		Node* child = children->at(name).get();

		// Sets the parent of the child to this node
		child->parent = this;

		// Returns the child
		return child;
	}
}