#include <iostream>

int main()
{
	int var1 = 2;
	int var2 = var1++;
	const int var3 = --var1;
	std::cout << var1 << "," << var2 << "," << var3;

	std::cin.get();
}
