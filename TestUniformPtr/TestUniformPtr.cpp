#define BOOST_TEST_MODULE TestUniformPtr

#include <boost/test/included/unit_test.hpp>

#include <memory>

#include "../uniform_ptr.hpp"

// used as base class
class IntValue {
public:
	virtual ~IntValue() {}
	virtual int getInt() const = 0;
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

	int getInt() const override { return m_value; }
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

	int getInt() const override { return m_value; }
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
	BOOST_CHECK(4 == akl::uniform_ptr<IntNonMovable>(IntNonMovable(4))->getInt()); // copying value
	BOOST_CHECK(6 == akl::uniform_ptr<IntValue>{IntNonMovable{ 6 }}->getInt());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_move_ctor)
{
	BOOST_CHECK(1 == *akl::uniform_ptr<int>{1}); // value is moved
	BOOST_CHECK('A' == *akl::uniform_ptr<char>{'A'});
	BOOST_CHECK(false == *akl::uniform_ptr<bool>{false});

	// checking with objects
	static_assert(!std::is_copy_constructible_v<IntNonCopyable>);
	BOOST_CHECK(7 == akl::uniform_ptr<IntNonCopyable>(IntNonCopyable(7))->getInt()); // moving value
	BOOST_CHECK(5 == akl::uniform_ptr<IntValue>(IntNonCopyable(5))->getInt()); // copying child value
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_ctor_from_pointer)
{
	int valInt = 6;
	BOOST_CHECK(valInt == *akl::uniform_ptr<decltype(valInt)>{&valInt});
	akl::uniform_ptr<decltype(valInt)> ptrValInt{ &valInt };
	BOOST_CHECK(valInt == *ptrValInt);
	++valInt;
	BOOST_CHECK(valInt == *ptrValInt);
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
	int b = 2;
	BOOST_CHECK(2 == *akl::uniform_ptr<int>{b}); // value is copyed
	BOOST_CHECK(2 == *akl::uniform_ptr<int>{&b}); // saving a pointer to value
	const int c = 3;
	BOOST_CHECK(3 == *akl::uniform_ptr<int>{c}); // ok cause the value is copyed
	//BOOST_CHECK(3 == *akl::uniform_ptr<int>{&c}); // is not compiled - const int != int
	BOOST_CHECK(3 == *akl::uniform_ptr<const int>{ c });

}