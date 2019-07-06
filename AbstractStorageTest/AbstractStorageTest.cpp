#include <iostream>
#include <variant>
#include <memory>

class StoredInt {
public:
	explicit StoredInt(const int val): mValue(val) {}
	explicit StoredInt(int * const val) : mValue(val) {}
	explicit StoredInt(std::shared_ptr<int> val) : mValue(val) {}
public:
	int & operator*()
	{
		if (auto pval = std::get_if<int>(&mValue))
		{
			return *pval;
		}
		else if (auto pval = std::get_if<int*>(&mValue))
		{
			return **pval;
		}
		else if (auto pval = std::get_if < std::shared_ptr<int>>(&mValue))
		{
			return **pval;
		};
	}
	const int & operator*() const
	{
		if (auto pval = std::get_if<int>(&mValue))
		{
			return *pval;
		}
		else if (auto pval = std::get_if<int*>(&mValue))
		{
			return **pval;
		}
		else if (auto pval = std::get_if < std::shared_ptr<int>>(&mValue))
		{
			return **pval;
		};
	}
private:
	std::variant<int, int *, std::shared_ptr<int> > mValue;
};

int main()
{
	StoredInt v1{ 3 };
	int b = 4;
	StoredInt v2{ &b };
	StoredInt v3{ std::make_shared<int>(8) };
    std::cout << "::: " << (*v1 + 2) << " ::: " << (*v2 + 7) << " ::: " << (*v3 + 12) << std::endl;
	return 0;
}

