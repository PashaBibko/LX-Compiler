#include <misc/fileRead.h>
#include <lexer/lexer.h>
#include <util/util.h>

#include <parser/ast.h>
#include <parser/parser.h>

#include <assembler/assembler.h>

#include <debug/DebugLog.h>

#include <string>
#include <filesystem>

#define DLL_FUNC extern "C" __declspec(dllexport)

namespace lx
{
	DLL_FUNC void Translate(const char* folder, const char* srcDir, const char* srcFileName, bool debugMode)
	{
		// Error checking
		// Explanation below why this is needed
		try
		{
			// Gets full file path
			std::string fullSrcFileName = std::string(folder) + std::string("/") + std::string(srcDir) + std::string("/") + std::string(srcFileName);

			// Read the file contents
			std::string fileContents = readFileToString(fullSrcFileName);

			// Creates all necessary objects
			Lexer lexer;
			Parser parser;
			Assembler assembler;

			std::vector<Token> tokens; // Output Tokens
			FileAST AST; // Output Abstract Syntax Tree

			// Lexical Analysis
			tokens = lexer.lex(fileContents);

			// Print tokens if in debug mode
			if (debugMode)
			{
				std::cout << "Tokens of: " + fullSrcFileName + "\n";
				DebugLog(tokens);
				std::cout << "\n";
			}

			// Parsing
			parser.parse(tokens, AST, debugMode);

			// Creates the output file (if it doesn't exist)
			std::string outputDir = std::string(folder) + std::string("/build");
			std::filesystem::create_directory(outputDir);

			// Creates the output file name
			std::string outputFileName = outputDir + std::string("/") + srcFileName + std::string(".cpp");

			// Opens the output file and writes the assembly code
			std::ofstream out(outputFileName);
			out << assembler.assemble(AST);
		}

		// Error handling
		// This is needed because the function is called from C#
		// and the C# debugger can't catch C++ exceptions
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}
