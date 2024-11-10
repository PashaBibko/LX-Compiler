#include <lexer.h>
#include <parser.h>
#include <assembler.h>

#include <macro/dll-func.h>

#include <unordered_map>

#include <fstream>
#include <sstream>

#define DLL_FUNC extern "C" DLL_EXPORT

namespace LX_API
{
	static std::string readFileToString(const std::string& filePath)
	{
		// Create an input file stream
		std::ifstream file(filePath);

		// Check if the file opened successfully
		if (!file.is_open()) {
			std::cerr << "Error opening file: " << filePath << std::endl;
			return "";
		}

		// Create a stringstream to hold the file contents
		std::stringstream buffer;
		buffer << file.rdbuf();

		// Return the contents as a string
		return buffer.str();
	}

	std::unordered_map<int, std::vector<lx::Token>> funcTokenMap;
	std::unordered_map<int, lx::FileAST> astMap;

	// Lexer function call
	DLL_FUNC void lexSource(const char* folder, const char* srcDir, const char* filename, bool debug)
	{
		std::string fullPath = std::string(folder) + "/" + std::string(srcDir) + "/" + std::string(filename);
		std::string source = readFileToString(fullPath);

		lx::Lexer lexer(source);

		funcTokenMap[0] = lexer.getFunctionTokens();

		if (debug)
		{
			for (const auto& token : funcTokenMap[0])
			{
				lx::DebugLog(token);
			}
		}
	}

	// Parser function call
	DLL_FUNC void parseTokens()
	{
		lx::Parser parser;

		parser.parse(funcTokenMap[0], astMap[0], true);
	}

	// Translator function call
	DLL_FUNC void assembleAST(const char* folder, const char* filename)
	{
		lx::Assembler assembler;

		std::string outDir = std::string(folder) + "/build";

		for (lx::FunctionDeclaration& func : astMap[0].functions)
		{
			assembler.assemble(func, outDir, filename);
		}
	}

	// Create header file
	DLL_FUNC void CreateHeaderFile(const char* folder)
	{
		std::ofstream headerFile(std::string(folder) + "/build/functions.h");

		headerFile << "#pragma once\n\n";

		for (std::string& header : lx::Assembler::funcHeaders)
		{
			headerFile << header << ";\n";
		}

		headerFile.close();
	}
}