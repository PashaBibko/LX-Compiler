#include <iostream>
int add(int a, int b)
{
	return a + b;
}
int main()
{
	std::cout << "hello";
	std::cout << add(1, 2);;
	if (true)
{
<<<<<<< Updated upstream
	std::cout << "all is normal";
}
else if (false)
{
	std::cout << "things are a bit weird";
}
else
{
	std::cout << "YOU ARE IN THE MATRIX";
=======
	std::cout << "Things are normal";
}
else if (false)
{
	std::cout << "Time to go home";
}
else
{
	std::cout << "What has gone on here";
>>>>>>> Stashed changes
}

}
