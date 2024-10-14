#pragma once

#include <assembler/lx-core.h>
#include <parser/parser.h>

#include <sstream>
#include <fstream>
#include <set>

namespace lx 
{
	class Assembler
	{
		public:
			static std::vector<std::string> funcList;
			static std::vector<std::string> funcHeaders;

			std::set<std::string> includes;
			std::ostringstream out;

			static std::unordered_map<ASTNode::NodeType, std::function<void(Assembler&, ASTNode*)>> nodeAssemblers;

			Assembler() = default;

			void assembleNode(ASTNode* node);

			void assemble(FunctionDeclaration& AST, const std::string outputDir, const std::string lx_fileName);
	};
}
