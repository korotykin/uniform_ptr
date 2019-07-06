#include <iostream>
#include <variant>
#include <memory>

template<typename T>
class Storage {
public:
	explicit Storage(const T & val): mValue(val) {}
	explicit Storage(T && val) : mValue(std::move(val)) {}
	explicit Storage(T * const val) : mValue(val) {}
	explicit Storage(std::shared_ptr<T> val) : mValue(val) {}
public:
	T & operator*()
	{
		if (auto pval = std::get_if<T>(&mValue))
		{
			return *pval;
		}
		else if (auto pval = std::get_if<T*>(&mValue))
		{
			return **pval;
		}
		else if (auto pval = std::get_if < std::shared_ptr<T>>(&mValue))
		{
			return **pval;
		};
	}
	const T & operator*() const
	{
		if (auto pval = std::get_if<T>(&mValue))
		{
			return *pval;
		}
		else if (auto pval = std::get_if<T*>(&mValue))
		{
			return **pval;
		}
		else if (auto pval = std::get_if < std::shared_ptr<T>>(&mValue))
		{
			return **pval;
		};
	}
private:
	std::variant<T, T *, std::shared_ptr<T> > mValue;
};

int main()
{
	Storage<int> v1{ 3 };
	int b = 4;
	Storage<int> v2{ &b };
	Storage<int> v3{ std::make_shared<int>(8) };
    std::cout << "::: " << (*v1 + 2) << " ::: " << (*v2 + 7) << " ::: " << (*v3 + 12) << std::endl;
	return 0;
}

