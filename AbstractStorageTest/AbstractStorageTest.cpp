#include <iostream>
#include "../uniform_ptr.hpp"

int main()
{
	int b = 4;
	std::cout << "::: " << (*uniform_ptr<int>{ 3 } +2) << std::endl
		<< " ::: " << (*uniform_ptr<int>{ &b } +7) << std::endl
		<< " ::: " << (*uniform_ptr<int>{ std::make_shared<int>(8)} +12) << std::endl
		<< " ::: " << (*uniform_ptr<int>{std::make_unique<int>(9)} +20) << std::endl;
	return 0;
}

