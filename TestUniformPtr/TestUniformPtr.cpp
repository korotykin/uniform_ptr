#define BOOST_TEST_MODULE TestUniformPtr

#include <boost/test/included/unit_test.hpp>

#include <memory>

#include "../uniform_ptr.hpp"

// used as base class
class IntValue {
public:
	virtual ~IntValue() {}
	virtual int get() const = 0;
};

// Copy ctor disabled. Move ctor enabled
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

// Move ctor disabled. Copy ctor enabled
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

BOOST_AUTO_TEST_CASE(test_uniform_ptr_default_ctor)
{
	BOOST_CHECK(nullptr == akl::uniform_ptr<char>{}.get());
	BOOST_CHECK(nullptr == akl::uniform_ptr<IntValue>{}.get());
	BOOST_CHECK(nullptr == akl::uniform_ptr<IntNonMovable>{}.get());
	BOOST_CHECK(nullptr == akl::uniform_ptr<IntNonCopyable>{}.get());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_nullptr_ctor)
{
	BOOST_CHECK(nullptr == akl::uniform_ptr<char>{nullptr}.get());
	BOOST_CHECK(nullptr == akl::uniform_ptr<IntValue>{nullptr}.get());
	BOOST_CHECK(nullptr == akl::uniform_ptr<IntNonMovable>{nullptr}.get());
	BOOST_CHECK(nullptr == akl::uniform_ptr<IntNonCopyable>{nullptr}.get());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_const_ref_ctor)
{
	const char A = 'A'; // it disallows using move ctor
	BOOST_CHECK(A == *akl::uniform_ptr<char>{A});
	const bool T = true;
	BOOST_CHECK(*akl::uniform_ptr<bool>{T});
	const long long V = 3;
	BOOST_CHECK(V == *akl::uniform_ptr<long long>{V});
	static_assert(!std::is_move_constructible_v<IntNonMovable>);
	BOOST_CHECK(4 == akl::uniform_ptr<IntNonMovable>(IntNonMovable(4))->get()); // copying value
	BOOST_CHECK(6 == akl::uniform_ptr<IntValue>{IntNonMovable{ 6 }}->get());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_bool_cast)
{
	BOOST_CHECK(false == (bool)akl::uniform_ptr<int>{});
	BOOST_CHECK(false == akl::uniform_ptr<int>{});
	BOOST_CHECK(!akl::uniform_ptr<int>{});
	BOOST_CHECK(false == (bool)akl::uniform_ptr<char>{nullptr});
	BOOST_CHECK(true == (bool)akl::uniform_ptr<bool>{false});
	BOOST_CHECK(true == akl::uniform_ptr<bool>{false});
	BOOST_CHECK(akl::uniform_ptr<bool>{false});
	BOOST_CHECK(false == (bool)akl::uniform_ptr<IntValue>{});
	BOOST_CHECK(false == (bool)akl::uniform_ptr<IntNonMovable>{});
	BOOST_CHECK(false == (bool)akl::uniform_ptr<IntNonCopyable>{nullptr});
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr)
{
	// checking with POD type = int
	BOOST_CHECK(1 == *akl::uniform_ptr<int>{1}); // value is moved
	int b = 2;
	BOOST_CHECK(2 == *akl::uniform_ptr<int>{b}); // value is copyed
	BOOST_CHECK(2 == *akl::uniform_ptr<int>{&b}); // saving a pointer to value
	const int c = 3;
	BOOST_CHECK(3 == *akl::uniform_ptr<int>{c}); // ok cause the value is copyed
	//BOOST_CHECK(3 == *akl::uniform_ptr<int>{&c}); // is not compiled - const int != int
	BOOST_CHECK(3 == *akl::uniform_ptr<const int>{ c });

	// checking with objects
	static_assert(!std::is_copy_constructible_v<IntNonCopyable>);
	BOOST_CHECK(7 == akl::uniform_ptr<IntNonCopyable>(IntNonCopyable(7))->get()); // moving value
	BOOST_CHECK(5 == akl::uniform_ptr<IntValue>(IntNonCopyable(5))->get()); // copying child value
}