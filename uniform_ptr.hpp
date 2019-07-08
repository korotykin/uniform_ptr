#pragma once

#ifndef _UNIFORM_PTR_HPP_

#include <variant>
#include <memory>

template<typename T>
class uniform_ptr {
public:
	uniform_ptr() : mValue(nullptr) {}
	uniform_ptr(const T & val) : mValue(std::make_unique<T>(val)) {}
	uniform_ptr(T && val) : mValue(std::make_unique<T>(std::move(val))) {}
	uniform_ptr(T * const val) : mValue(val) {}
	uniform_ptr(std::shared_ptr<T> val) : mValue(val) {}
	uniform_ptr(std::unique_ptr<T> val) : mValue(std::move(val)) {}

	template<typename C>
	uniform_ptr(const C & val) : mValue(std::unique_ptr<T>(std::make_unique<C>(val))) {}
	template<typename C>
	uniform_ptr(C && val) : mValue(std::unique_ptr<T>(std::make_unique<C>(std::move(val)))) {}
	template<typename C>
	uniform_ptr(C * const val) : mValue((T*)val) {}
	template <typename C>
	uniform_ptr(std::shared_ptr<C> val) : mValue(val) {}
	template <typename C>
	uniform_ptr(std::unique_ptr<C> val) : mValue(std::move(std::unique_ptr<T>(std::move(val)))) {}

	uniform_ptr(const uniform_ptr<T> & lhv) = delete;
	uniform_ptr(uniform_ptr<T> && lhv) noexcept : mValue(std::move(lhv.mValue)) {}
	uniform_ptr<T> & operator=(const uniform_ptr<T> & lhv) = delete;
	uniform_ptr<T> & operator=(uniform_ptr<T> && lhv) noexcept
	{
		mValue = std::move(lhv.mValue);
	}
	~uniform_ptr() = default;
public:
	operator bool() const { return getPointer() != nullptr; }
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
	std::variant<nullptr_t, T *, std::shared_ptr<T>, std::unique_ptr<T> > mValue;

	T * getPointer()
	{
		if (auto pval = std::get_if<nullptr_t>(&mValue))
		{
			return *pval;
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
		if (auto pval = std::get_if<nullptr_t>(&mValue))
		{
			return *pval;
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

#endif // !_UNIFORM_PTR_HPP_
