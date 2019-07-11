#define BOOST_TEST_MODULE TestUniformPtr

#include <boost/test/included/unit_test.hpp>

#include <memory>

#include "../uniform_ptr.hpp"

class IntValue {
public:
	virtual ~IntValue() {}
	virtual int get() const = 0;
};

class IntNonCopyable final: public IntValue {
public:
	IntNonCopyable() = delete;
	explicit IntNonCopyable(int a_value) : m_value(a_value) {}
	IntNonCopyable(const IntNonCopyable &) = delete;
	IntNonCopyable(IntNonCopyable &&) = default;
	IntNonCopyable & operator=(const IntNonCopyable &) = delete;
	IntNonCopyable & operator=(IntNonCopyable &&) = default;
	~IntNonCopyable() = default;

	int get() const override { return m_value; }
private:
	int m_value = 0;
};

class IntNonMovable final: public IntValue {
public:
	IntNonMovable() = delete;
	explicit IntNonMovable(int a_value) : m_value(a_value) {}
	IntNonMovable(const IntNonMovable &) = default;
	IntNonMovable(IntNonMovable &&) = delete;
	IntNonMovable & operator=(const IntNonMovable &) = default;
	IntNonMovable & operator=(IntNonMovable &&) = delete;
	~IntNonMovable() = default;

	int get() const override { return m_value; }
private:
	int m_value = 0;
};

BOOST_AUTO_TEST_CASE(test_uniform_ptr)
{
	BOOST_CHECK(false == (bool)uniform_ptr<int>{});
	BOOST_CHECK(1 == *uniform_ptr<int>{1});
	static_assert(!std::is_move_constructible_v<IntNonMovable>);
	BOOST_CHECK(2 == uniform_ptr<IntNonMovable>(IntNonMovable(2))->get()); // copying value
	static_assert(!std::is_copy_constructible_v<IntNonCopyable>);
	BOOST_CHECK(3 == uniform_ptr<IntNonCopyable>(IntNonCopyable(3))->get()); // moving value
	BOOST_CHECK(3 == uniform_ptr<IntValue>(IntNonMovable(3))->get()); // copying child value
}