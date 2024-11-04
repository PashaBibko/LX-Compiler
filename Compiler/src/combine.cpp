#include <lexer/lexer.h>

#include <parser/ast.h>
#include <parser/parser.h>

#include <assembler/assembler.h>

#include <debug/DebugLog.h>

#include <string>
#include <filesystem>

#define DLL_FUNC extern "C" __declspec(dllexport)

namespace lx
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
			Lexer lexer (fileContents); // Output tokens

			Parser parser;

			const std::vector<Token> tokens;
			FileAST AST; // Output Abstract Syntax Tree

			// Print tokens if in debug mode
			if (debugMode)
			{
				std::cout << "Tokens of: " + fullSrcFileName + "\n";
				DebugLog(tokens);
				std::cout << "\n";
			}

			return;

			// Parsing
			parser.parse(tokens, AST, debugMode);

			// Creates the output folder (if it doesn't exist)
			std::string outputDir = std::string(folder) + std::string("/build");
			std::filesystem::create_directory(outputDir);

			std::cout << "Functions: " << AST.functions.size() << "\n";

			// Loops through all functions and assembles them
			for (FunctionDeclaration& func : AST.functions)
			{
				Assembler assembler;

				assembler.assemble(func, outputDir, srcFileName);
				std::cout << "Function: " << func.name.name << " assembled\n";
			}
		}

		// Error handling
		// This is needed because the function is called from C#
		// and the C# debugger can't catch C++ exceptions
		catch (const std::exception& e)
		{
			std::cerr << "C++ ERROR: " << e.what() << std::endl;
		}

		catch (const lx::Error& e)
		{
			e.display();
		}

		catch (...) // Catch all
		{
			std::cerr << "An unknown error occurred" << std::endl;
		}
	}

	DLL_FUNC void CreateHeaderFile(const char* folder)
	{
		std::ofstream headerFile(folder + std::string("/build/functions.h"));

		std::cout << "Creating header file: " << folder + std::string("/build/functions.h") << "\n";

		if (headerFile.is_open() == false)
		{
			std::cerr << "Failed to create header file\n";
			return;
		}

		headerFile << "#pragma once\n\n";

		for (std::string& funcHeader : Assembler::funcHeaders)
		{
			headerFile << funcHeader << ";\n";
		}

		headerFile.close();
	}
}
