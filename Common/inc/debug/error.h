#pragma once

#include <filesystem>
#include <iostream>
#include <string>

#define NL "\n"

namespace LX::Debug
{
	struct Error
	{
		unsigned int lineNum;

		std::string fileName;
		std::string errorMessage;

		Error(const char* fullFileName, unsigned int lineNum, const std::string& errorMessage) :
			lineNum(lineNum), errorMessage(errorMessage)
		{
			std::filesystem::path path = fullFileName;
			fileName = path.filename().string();
		}

		void display() const
		{
			std::cerr << "LX ERROR: " << NL;
			std::cerr << "Thrown from: " << fileName << " at line: " << lineNum << "\n";
			std::cerr << "Message: " << errorMessage << "\n\n";
		}
	};
}

#define THROW_ERROR(msg) throw LX::Debug::Error(__FILE__, __LINE__, msg)
