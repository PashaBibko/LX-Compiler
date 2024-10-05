#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace lx
{
	inline std::string readFileToString(const std::string& filePath)
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
}