#include <lexer.h>
#include <parser.h>
#include <translator.h>

#include <macro/dll-func.h>

#include <unordered_map>

#include <fstream>
#include <sstream>
#include <cstdlib>

#define DLL_FUNC extern "C" DLL_EXPORT

namespace LX::API
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

	std::unordered_map<int, std::vector<LX::Lexer::Token>> funcTokenMap;
	std::unordered_map<int, LX::Parser::FileAST> astMap;

	// Lexer function call
	DLL_FUNC int lexSource(const char* folder, const char* srcDir, const char* filename, bool debug)
	{
		// Main lexer function call
		try
		{
			// Gets the full path of the file
			std::string fullPath = std::string(folder) + "/" + std::string(srcDir) + "/" + std::string(filename);
			std::string source = readFileToString(fullPath);

			// Creates a lexer object
			LX::Lexer::Lexer lexer(source, debug);

			// Gets the next id
			int id = (int)funcTokenMap.size();

			// Moves the tokens to the map from the lexer
			// Probably should optimize this
			funcTokenMap[id] = lexer.getFunctionTokens();

			// Debug CLI
			if (debug == true)
			{
				while (true)
				{
					// Asks the user what debug option they want
					std::string userInput;

					std::cout << "Lexer debug options:\n";
					std::cout << "1. Display sections\n";
					std::cout << "2. Display function tokens\n";
					std::cout << "Press enter to continue: ";

					std::getline(std::cin, userInput);

					system("cls");

					if (userInput == "2")
					{
						for (const LX::Lexer::Token& token : funcTokenMap[id])
						{
							LX::Debug::Log(token);
						}
					}

					else if (userInput == "1")
					{
						for (Lexer::LexerStreamSect& sect : lexer.getSections())
						{
							sect.debugDisplay();
						}
					}

					else
					{
						system("cls");
						break;
					}

					std::cout << "\n\nPress enter to continue: ";
					std::string temp = "";
					std::getline(std::cin, temp);

					system("cls");
				}
			}
			
			// Returns the src id
			return id;
		}

		// C++ error handling
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return -1;
		}

		// LX error handling
		catch (const LX::Debug::Error& e)
		{
			e.display();
			return -1;
		}
	}

	// Parser function call
	DLL_FUNC bool parseTokens(bool debug, int id)
	{
		// Main parser function call
		try
		{
			LX::Parser::Parser parser;

			parser.parse(funcTokenMap[id], astMap[id]);

			return true;
		}

		// C++ error handling
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}

		// LX error handling
		catch (const LX::Debug::Error& e)
		{
			e.display();
			return false;
		}
	}

	// Translator function call
	DLL_FUNC bool translateAST(const char* folder, const char* filename, bool debug, int id)
	{
		// Main translator function call
		try
		{
			LX::Translator::Translator translator;

			std::string outDir = std::string(folder) + "/build";

			for (LX::Parser::FunctionDeclaration& func : astMap[id].functions)
			{
				translator.assemble(func, outDir, filename);
			}

			return true;
		}

		// C++ error handling
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}

		// LX error handling
		catch (const LX::Debug::Error& e)
		{
			e.display();
			return false;
		}
	}

	// Create header file
	DLL_FUNC bool CreateHeaderFile(const char* folder)
	{
		try
		{
			std::ofstream headerFile(std::string(folder) + "/build/functions.h");

			headerFile << "#pragma once\n\n";

			for (std::string& header : LX::Translator::Translator::funcHeaders)
			{
				headerFile << header << ";\n";
			}

			headerFile.close();

			return true;
		}

		// C++ error handling
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}

		// LX error handling
		catch (const LX::Debug::Error& e)
		{
			e.display();
			return false;
		}
	}
}