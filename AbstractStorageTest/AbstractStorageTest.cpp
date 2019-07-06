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
	explicit Storage(std::unique_ptr<T> val) : mValue(std::move(val)) {} // using std::move to resolve issue
public:
	T & operator*()
	{
		return *getPointer();
	}
	const T & operator*() const
	{
		return *getPointer();
	}
	T * operator->()
	{
		return getPointer();
	}
	const T * operator->() const
	{
		return getPointer();
	}
private:
	std::variant<T, T *, std::shared_ptr<T>, std::unique_ptr<T> > mValue;

	T * getPointer()
	{
		if (auto pval = std::get_if<T>(&mValue))
		{
			return pval;
		}
		else if (auto pval = std::get_if<T*>(&mValue))
		{
			return *pval;
		}
		else if (auto pval = std::get_if < std::shared_ptr<T>>(&mValue))
		{
			return pval->get();
		}
		else if (auto pval = std::get_if <std::unique_ptr<T>>(&mValue))
		{
			return pval->get();
		};
		throw std::logic_error("invalid type");
	}
	const T * getPointer() const
	{
		if (auto pval = std::get_if<T>(&mValue))
		{
			return pval;
		}
		else if (auto pval = std::get_if<T*>(&mValue))
		{
			return *pval;
		}
		else if (auto pval = std::get_if < std::shared_ptr<T>>(&mValue))
		{
			return pval->get();
		}
		else if (auto pval = std::get_if <std::unique_ptr<T>>(&mValue))
		{
			return pval->get();
		};
		throw std::logic_error("invalid type");
	}

};

int main()
{
	int b = 4;
	Storage<int> v3{ std::make_shared<int>(8) };
	std::cout << "::: " << (*Storage<int>{ 3 } +2) << std::endl
		<< " ::: " << (*Storage<int>{ &b } +7) << std::endl
		<< " ::: " << (*Storage<int>{ std::make_shared<int>(8)} +12) << std::endl
		<< " ::: " << (*Storage<int>{std::make_unique<int>(9)} +20) << std::endl;
	return 0;
}

