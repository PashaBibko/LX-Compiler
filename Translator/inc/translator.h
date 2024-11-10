#pragma once

#include <lx-core.h>
#include <cdt/ast.h>

#include <sstream>
#include <fstream>
#include <set>

namespace LX::Translator
{
	class Translator
	{
		public:
			static std::vector<std::string> funcList;
			static std::vector<std::string> funcHeaders;

			std::set<std::string> includes;
			std::ostringstream out;

			static std::unordered_map<LX::Parser::ASTNode::NodeType, std::function<void(Translator&, LX::Parser::ASTNode*)>> nodeTranslators;

			Translator() = default;

			void assembleNode(LX::Parser::ASTNode* node);

			void assemble(LX::Parser::FunctionDeclaration& AST, const std::string outputDir, const std::string lx_fileName);
	};
}
