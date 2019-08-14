#pragma once

#ifndef _UNIFORM_PTR_HPP_

#include <functional>
#include <memory>
#include <type_traits>

template<typename T>
class uniform_ptr {
public:
	uniform_ptr(nullptr_t = nullptr) : mF([]() -> T* { return nullptr; }) {}

	template<typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>, int> = 0 >
	uniform_ptr(const U & val) : mF([p = val]() mutable ->T* { return &p; }) {}

	template<typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*> && std::is_move_constructible_v<U> && !std::is_reference_v<U>, int> = 0>
	uniform_ptr(U&& val) : mF([p = std::forward<U>(val)]() mutable ->T* { return &p; }
	)
	{
	}

	template<typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(U* const val) : mF([p = val]() -> T* {return p; }) {}

	template <typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(std::shared_ptr<U> val) : mF([p = val]() -> T* { return p.get();  })
	{
	}

	template <typename U = T, std::enable_if_t<std::is_convertible_v<U*, T*>, int> = 0>
	uniform_ptr(std::unique_ptr<U> val) : mF(std::bind([](std::unique_ptr<U>& p) -> T* { return p.get(); }, std::move(val)))
	{
	}

	// copy and move ctors
	uniform_ptr(const uniform_ptr<T>& lhv) = delete; // delete <- because we use unique_ptr in implementation
	uniform_ptr(uniform_ptr<T>&& lhv) noexcept : mF(std::move(lhv.mF)) {}
	uniform_ptr<T>& operator=(const uniform_ptr<T>& lhv) = delete;
	uniform_ptr<T>& operator=(uniform_ptr<T>&& lhv) noexcept
	{
		mF = std::move(lhv.mF);
	}

	~uniform_ptr() = default; // non virtual <- inheritance is possible, but I don't see any reason to have 'pointer to pointer'
public:
	operator bool() const { return get() != nullptr; }
	T& operator*()
	{
		return *get();
	}
	const T& operator*() const
	{
		return *get();
	}
	T* operator->()
	{
		return get();
	}
	const T* operator->() const
	{
		return get();
	}

	T* get()
	{
		return mF();
	}
	const T* get() const
	{
		return mF();
	}
private:
	std::function<T* ()> mF;
};

#endif // !_UNIFORM_PTR_HPP_
