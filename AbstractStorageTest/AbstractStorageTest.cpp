#include <iostream>
#include <variant>
#include <memory>

template<typename T>
class uniform_ptr {
public:
	explicit uniform_ptr(const T & val): mValue(val) {}
	explicit uniform_ptr(T && val) : mValue(std::move(val)) {}
	explicit uniform_ptr(T * const val) : mValue(val) {}
	explicit uniform_ptr(std::shared_ptr<T> val) : mValue(val) {}
	explicit uniform_ptr(std::unique_ptr<T> val) : mValue(std::move(val)) {} // using std::move to resolve issue
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
	std::cout << "::: " << (*uniform_ptr<int>{ 3 } +2) << std::endl
		<< " ::: " << (*uniform_ptr<int>{ &b } +7) << std::endl
		<< " ::: " << (*uniform_ptr<int>{ std::make_shared<int>(8)} +12) << std::endl
		<< " ::: " << (*uniform_ptr<int>{std::make_unique<int>(9)} +20) << std::endl;
	return 0;
}

