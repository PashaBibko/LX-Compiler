// ======================================================================================= //
//                                                                                         //
// This code is license under a Proprietary License for LX - Compiler                      //
//                                                                                         //
// Copyright(c) 2024 Pasha Bibko                                                           //
//                                                                                         //
// 1. License Grant                                                                        //
//     You are granted a non - exclusive, non - transferable, and revocable                //
//     license to use this software for personal, educational, non - commercial,           //
//     or internal commercial purposes.You may install and use the software on             //
//     your devices or within your company, but you may not sell, sublicense,              //
//     or distribute the software in any form, either directly or as part                  //
//     of any derivative works. You may privately modify the software for                  //
//     internal use within your organization, provided that the modified versions          //
//     are not distributed, shared, or otherwise made available to third parties.          //
//                                                                                         //
// 2. Freedom to Share Creations                                                           //
//     You are free to create, modify, and share works or creations made with this         //
//     software, provided that you do not redistribute the original software itself.       //
//     All creations made with this software are solely your responsibility, and           //
//     you may license or distribute them as you wish, under your own terms.               //
//                                                                                         //
// 3. Restrictions                                                                         //
//     You may not:                                                                        //
//     - Sell, rent, lease, or distribute the original software or any copies              //
//       thereof, including modified versions.                                             //
//     - Distribute the software or modified versions to any third party.                  //
//                                                                                         //
// 4. Disclaimer of Warranty                                                               //
//     This software is provided "as is", without warranty of any kind, either             //
//     express or implied, including but not limited to the warranties of merchantability, //
//     fitness for a particular purpose, or non - infringement.In no event shall the       //
//     authors or copyright holders be liable for any claim, damages, or other liability,  //
//     whether in an action of contract, tort, or otherwise, arising from, out of, or in   //
//     connection with the software or the use or other dealings in the software.          //
//                                                                                         //
// ======================================================================================= //

#include <common.h>

#include <lexer.h>
#include <parser.h>
#include <translator.h>

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

			if (debug == true)
			{
				for (LX::Parser::FunctionDeclaration& func : astMap[id].functions)
				{
					Debug::Log(&func, 0);
				}
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

	// Translator function call
	DLL_FUNC bool translateAST(const char* folder, const char* filename, bool debug, int id)
	{
		// Main translator function call
		try
		{
			std::string outDir = std::string(folder) + "/build";

			for (LX::Parser::FunctionDeclaration& func : astMap[id].functions)
			{
				LX::Translator::Translator translator;

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