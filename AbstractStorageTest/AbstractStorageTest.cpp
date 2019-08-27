#include "../uniform_ptr.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

class Outputer
{
public:
	Outputer & add_stream(akt::uniform_ptr<std::ostream> && a_ostream);
	template <typename T>
	Outputer & operator<<(const T & val);
private:
	std::vector<akt::uniform_ptr<std::ostream> > m_ostrs;
};

int main()
{
	Outputer outter;
	outter.add_stream(&std::cout);
	outter.add_stream(std::make_shared<std::ofstream>("local2.txt", std::ios::trunc));
	outter.add_stream(std::make_unique<std::fstream>("local1.txt", std::ios::out));
	outter.add_stream(std::ofstream("local3.txt", std::ios::out | std::ios::ate));
	std::fstream file4("local4.txt", std::ios::out | std::ios::ate);
	outter.add_stream(std::move(file4)); // explicit using move because fstream is not copyable
	outter << "Hello world!\n";

	return 0;
}

Outputer & Outputer::add_stream(akt::uniform_ptr<std::ostream> && a_ostream)
{
	m_ostrs.emplace_back(std::move(a_ostream));
	return *this;
}

template <typename T>
Outputer & Outputer::operator<<(const T & val)
{
	for (auto & ostr : m_ostrs)
	{
		if (bool(ostr) == true)
		{
			if (ostr->fail() != true)
			{
				*ostr << val;
			}
			else
			{
				std::cerr << "invalid stream" << std::endl;
			}
		}
		else
		{
			std::cerr << "failed to out value" << std::endl;
		}
	}
	return *this;
}


