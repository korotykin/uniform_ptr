#pragma once

#ifndef _UNIFORM_PTR_HPP_

#include <variant>
#include <memory>
#include <type_traits>

template<typename T>
class uniform_ptr {
public:
	uniform_ptr() : mValue(nullptr) {}

	template<typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>, int> = 0 >
	uniform_ptr(const U & val) : mValue(std::unique_ptr<T>(std::make_unique<U>(val))) {}

	template<typename U = T, std::enable_if_t<std::is_move_constructible_v<U>, int> = 0>
	uniform_ptr(U && val) : mValue(std::unique_ptr<T>(std::make_unique<U>(std::move(val)))) {}

	template<typename U = T>
	uniform_ptr(U * const val) : mValue(val) {}

	uniform_ptr(std::shared_ptr<T> val) : mValue(val) {}
	uniform_ptr(std::unique_ptr<T> val) : mValue(std::move(val)) {}

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
