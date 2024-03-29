#pragma once

#ifndef _UNIFORM_PTR_HPP_

#include <functional>
#include <memory>
#include <type_traits>

namespace akt {

template<typename T>
class uniform_ptr {
public:
	uniform_ptr(nullptr_t = nullptr) : mF([]() -> T* { return nullptr; }) {}

	// makes a copy of original value
	template<typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>, int> = 0 >
	uniform_ptr(const U & val) : mF([p = std::make_shared<U>(val)]()  -> T* { return p.get(); }) {}

	template<typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*> && std::is_move_constructible_v<U> && !std::is_reference_v<U>, int> = 0>
	uniform_ptr(U&& val) : mF([p = std::make_shared<U>(std::forward<U>(val))]() ->T* { return p.get(); }) {}

	template<typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(U* const val) : mF([p = val]() -> T* { return p; }) {}

	template <typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(std::shared_ptr<U> val) : mF([p = val]() -> T* { return p.get(); }) {}

	template <typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(std::unique_ptr<U> val) : mF([p = std::shared_ptr(std::move(val))]() -> T* { return p.get(); }) {}

	// copy and move ctors
	uniform_ptr(const uniform_ptr<T>& rhv) = default;
	uniform_ptr(uniform_ptr<T>&& rhv) noexcept : mF(std::move(rhv.mF)) {}
	uniform_ptr<T>& operator=(const uniform_ptr<T>& rhv) = default;
	uniform_ptr<T>& operator=(uniform_ptr<T>&& rhv) noexcept
	{
		if (this != &rhv)
		{
			mF = std::move(rhv.mF);
		}
		return *this;
	}

	template<typename U, std::enable_if_t<!std::is_same_v<U, T> && std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(const uniform_ptr<U>& rhv) : mF([p = rhv]() mutable -> T* {return p.get(); }) {}

	template<typename U, std::enable_if_t<!std::is_same_v<U, T> && std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(uniform_ptr<U>&& rhv) noexcept : mF([p = std::forward<uniform_ptr<U>>(rhv)]() mutable -> T* {return p.get(); }) {}

	template<typename U, std::enable_if_t<!std::is_same_v<U, T> && std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr<T>& operator=(const uniform_ptr<U>& rhv)
	{
		mF = [p = rhv]() mutable -> T* {return p.get(); };
		return *this;
	}

	template<typename U, std::enable_if_t<!std::is_same_v<U, T> && std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr<T>& operator=(uniform_ptr<U>&& rhv)
	{
		mF = [p = std::move(rhv)]() mutable -> T* {return p.get(); };
		return *this;
	}

	~uniform_ptr() = default; // non virtual <- inheritance is possible, but I don't see any reason to have 'pointer to pointer'
public:
	operator bool() const { return get() != nullptr; }
	T& operator*() const
	{
		return *get();
	}
	T* operator->() const
	{
		return get();
	}

	T* get() const
	{
		return mF();
	}
private:
	std::function<T*(void)> mF;
};

}

#endif // !_UNIFORM_PTR_HPP_
