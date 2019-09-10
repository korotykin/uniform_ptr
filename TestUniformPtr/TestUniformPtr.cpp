#define BOOST_TEST_MODULE TestUniformPtr

#include <boost/test/included/unit_test.hpp>

#include <memory>

#include "../uniform_ptr.hpp"

// used as base class
class IntValue {
public:
	virtual ~IntValue() {}
	virtual int getInt() const = 0;
	virtual void setInt(int val) = 0;
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
	void setInt(int val) override { m_value = val; }
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
	void setInt(int val) override { m_value = val; }
private:
	int m_value = 0;
};

BOOST_AUTO_TEST_CASE(test_uniform_ptr_default_ctor)
{
	BOOST_CHECK(nullptr == akt::uniform_ptr<char>{}.get());
	BOOST_CHECK(nullptr == akt::uniform_ptr<IntValue>{}.get());
	BOOST_CHECK(nullptr == akt::uniform_ptr<IntNonMovable>{}.get());
	BOOST_CHECK(nullptr == akt::uniform_ptr<IntNonCopyable>{}.get());
	{
		const akt::uniform_ptr<int> p;
		BOOST_CHECK_EQUAL(nullptr, p.get());
		BOOST_CHECK_EQUAL(false, (bool)p);
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_nullptr_ctor)
{
	BOOST_CHECK(nullptr == akt::uniform_ptr<char>{nullptr}.get());
	BOOST_CHECK(nullptr == akt::uniform_ptr<IntValue>{nullptr}.get());
	BOOST_CHECK(nullptr == akt::uniform_ptr<IntNonMovable>{nullptr}.get());
	BOOST_CHECK(nullptr == akt::uniform_ptr<IntNonCopyable>{nullptr}.get());
	{
		const akt::uniform_ptr<int> p{ nullptr };
		BOOST_CHECK_EQUAL(nullptr, p.get());
		BOOST_CHECK_EQUAL(false, (bool)p);
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_const_ref_value_ctor)
{
	{
		const char A = 'A'; // it disallows using move ctor
		BOOST_CHECK_EQUAL('A', *akt::uniform_ptr<char>{A});
	}
	{
		const char A = 'A'; // it disallows using move ctor
		BOOST_CHECK_EQUAL('A', *akt::uniform_ptr<const char>{A});
	}
	{
		const bool T = true;
		BOOST_CHECK_EQUAL(true, *akt::uniform_ptr<bool>{T});
	}
	{
		const bool T = true;
		BOOST_CHECK_EQUAL(true, *akt::uniform_ptr<const bool>{T});
	}
	{
		const long long V = 3;
		BOOST_CHECK_EQUAL(3LL, *akt::uniform_ptr<long long>{V});
	}
	{
		const long long V = 3;
		BOOST_CHECK_EQUAL(3LL, *akt::uniform_ptr<const long long>{V});
	}
	static_assert(!std::is_move_constructible_v<IntNonMovable>);
	BOOST_CHECK(4 == akt::uniform_ptr<IntNonMovable>(IntNonMovable(4))->getInt()); // copying value
	BOOST_CHECK(4 == akt::uniform_ptr<const IntNonMovable>(IntNonMovable(4))->getInt());
	BOOST_CHECK(6 == akt::uniform_ptr<IntValue>{IntNonMovable{ 6 }}->getInt());
	BOOST_CHECK(6 == akt::uniform_ptr<const IntValue>{IntNonMovable{ 6 }}->getInt());
	{
		const char A = 'A';
		const akt::uniform_ptr<char> p{ A };
		BOOST_CHECK_EQUAL('A', *p);
		*p = 'B';
		BOOST_CHECK_EQUAL('B', *p);
	}
	{
		const char A = 'A';
		const akt::uniform_ptr<const char> p{ A };
		BOOST_CHECK_EQUAL('A', *p);
		static_assert(std::is_const<std::remove_reference_t<decltype(*p)>>::value, "*p is constant"); // check that *p is reference to const
		static_assert(std::is_same<decltype(*p), const char&>::value, "*p is 'const char&'"); // this line is not mandatory
		static_assert(!std::is_assignable<decltype(*p), char>::value, "*p is not assignable"); //check that *p = 'B' rises compilation error
	}
	{
		const akt::uniform_ptr<IntValue> p{ IntNonMovable{ 7 } };
		BOOST_CHECK_EQUAL(7, p->getInt());
		p->setInt(8);
		BOOST_CHECK_EQUAL(8, p->getInt());
	}
	{
		const akt::uniform_ptr<const IntValue> p{ IntNonMovable{ 7 } };
		BOOST_CHECK_EQUAL(7, p->getInt());
		static_assert(std::is_const<std::remove_reference_t<decltype(*p)>>::value, "*p is constant"); //check if *p is const
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_move_value_ctor)
{
	BOOST_CHECK(1 == *akt::uniform_ptr<int>{1}); // value is moved
	BOOST_CHECK('A' == *akt::uniform_ptr<char>{'A'});
	BOOST_CHECK(false == *akt::uniform_ptr<bool>{false});

	// checking with objects
	static_assert(!std::is_copy_constructible_v<IntNonCopyable>);
	BOOST_CHECK(7 == akt::uniform_ptr<IntNonCopyable>(IntNonCopyable(7))->getInt()); // moving value
	BOOST_CHECK(5 == akt::uniform_ptr<IntValue>(IntNonCopyable(5))->getInt()); // copying child value
	{
		const akt::uniform_ptr<IntValue> p{ IntNonCopyable{8} };
		BOOST_CHECK_EQUAL(8, p->getInt());
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_ctor_from_pointer)
{
	int valInt = 6;
	BOOST_CHECK(valInt == *akt::uniform_ptr<int>{&valInt});
	akt::uniform_ptr<int> ptrValInt{ &valInt };
	BOOST_CHECK(6 == *ptrValInt);
	++valInt;
	BOOST_CHECK(7 == *ptrValInt);

	IntNonCopyable objInt1{ 1 };
	BOOST_CHECK(1 == objInt1.getInt()); // check if value created
	akt::uniform_ptr<IntNonCopyable> p1ObjInt1{ &objInt1 }; // create 1st pointer to value
	BOOST_CHECK(1 == p1ObjInt1->getInt()); // check if 1st pointer created
	akt::uniform_ptr<IntValue> p2ObjInt1{ &objInt1 }; // create a pointer to value as pointer to parent class
	BOOST_CHECK(1 == p2ObjInt1->getInt()); // check if pointer created
	objInt1.setInt(2); // set new value
	BOOST_CHECK(2 == objInt1.getInt()); // check if value changed
	BOOST_CHECK(2 == p1ObjInt1->getInt());
	BOOST_CHECK(2 == p2ObjInt1->getInt());

	IntNonMovable objInt2{ 3 };
	BOOST_CHECK(3 == objInt2.getInt()); // check if value created
	akt::uniform_ptr<IntNonMovable> p1ObjInt2{ &objInt2 }; // create 1st pointer to value
	BOOST_CHECK(3 == p1ObjInt2->getInt()); // check if 1st pointer created
	akt::uniform_ptr<IntValue> p2ObjInt2{ &objInt2 }; // create a pointer to value as pointer to parent class
	BOOST_CHECK(3 == p2ObjInt2->getInt()); // check if pointer created
	objInt2.setInt(4); // set new value
	BOOST_CHECK(4 == objInt2.getInt()); // check if value changed
	BOOST_CHECK(4 == p1ObjInt2->getInt());
	BOOST_CHECK(4 == p2ObjInt2->getInt());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_ctor_from_shared_ptr)
{
	std::shared_ptr<int> val = std::make_shared<int>(6);
	BOOST_CHECK(6 == *akt::uniform_ptr<int>(val));
	akt::uniform_ptr<int> ptr{ val };
	BOOST_CHECK(6 == *ptr);
	*val = 9;
	BOOST_CHECK(9 == *val);
	BOOST_CHECK(9 == *ptr);

	std::shared_ptr<IntNonCopyable> p1ObjInt1 = std::make_shared<IntNonCopyable>(1);
	BOOST_CHECK(1 == p1ObjInt1->getInt()); // check if value created and assigned
	akt::uniform_ptr<IntNonCopyable> p2ObjInt1{ p1ObjInt1 }; // create 2nd pointer to value
	BOOST_CHECK(1 == p2ObjInt1->getInt()); // check if 2nd pointer created
	akt::uniform_ptr<IntValue> p3ObjInt1{ p1ObjInt1 }; // create a pointer to value as pointer to parent class
	BOOST_CHECK(1 == p3ObjInt1->getInt()); // check if pointer created
	p1ObjInt1->setInt(2); // set new value
	BOOST_CHECK(2 == p1ObjInt1->getInt()); // check if value changed
	BOOST_CHECK(2 == p2ObjInt1->getInt());
	BOOST_CHECK(2 == p3ObjInt1->getInt());

	std::shared_ptr<IntNonMovable> p1ObjInt2 = std::make_shared<IntNonMovable>(3);
	BOOST_CHECK(3 == p1ObjInt2->getInt()); // check if value created
	akt::uniform_ptr<IntNonMovable> p2ObjInt2{ p1ObjInt2 }; // create uniform pointer to value
	BOOST_CHECK(3 == p2ObjInt2->getInt()); // check if 2nd pointer created
	akt::uniform_ptr<IntValue> p3ObjInt2{ p1ObjInt2 }; // create a pointer to value as pointer to parent class
	BOOST_CHECK(3 == p3ObjInt2->getInt()); // check if pointer created
	p1ObjInt2->setInt(4); // set new value
	BOOST_CHECK(4 == p1ObjInt2->getInt()); // check if value changed
	BOOST_CHECK(4 == p2ObjInt2->getInt());
	BOOST_CHECK(4 == p3ObjInt2->getInt());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_ctor_from_unique_ptr)
{
	BOOST_CHECK(5 == *akt::uniform_ptr<int>(std::make_unique<int>(5)));
	BOOST_CHECK(6 == akt::uniform_ptr<IntNonCopyable>(std::make_unique<IntNonCopyable>(6))->getInt());
	BOOST_CHECK(7 == akt::uniform_ptr<IntValue>(std::make_unique<IntNonCopyable>(7))->getInt());
	BOOST_CHECK(8 == akt::uniform_ptr<IntValue>(std::unique_ptr<IntValue>(new IntNonCopyable{ 8 }))->getInt());
	BOOST_CHECK(9 == akt::uniform_ptr<IntNonMovable>(std::make_unique<IntNonMovable>(9))->getInt());
	BOOST_CHECK(10 == akt::uniform_ptr<IntValue>(std::make_unique<IntNonMovable>(10))->getInt());
	BOOST_CHECK(11 == akt::uniform_ptr<IntValue>(std::unique_ptr<IntValue>(new IntNonMovable{ 11 }))->getInt());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_copy_ctor)
{
	{
		akt::uniform_ptr<int> p;
		BOOST_CHECK_EQUAL(p.get(), akt::uniform_ptr<int>{ p }.get());
		BOOST_CHECK_EQUAL(false, (bool)akt::uniform_ptr<int>{ p });
	}

	{
		akt::uniform_ptr<int> p1{ 12 }; // take value by moving
		akt::uniform_ptr<int> p2{ p1 };
		BOOST_CHECK((bool)p2);
		BOOST_CHECK_EQUAL(12, *p2);
		*p1 = 14;
		BOOST_CHECK_EQUAL(14, *p2);
	}

	{
		int i = 15;
		akt::uniform_ptr<int> p1{ i }; // take value by copying
		akt::uniform_ptr<int> p2{ p1 };
		BOOST_CHECK_EQUAL(true, (bool)p2);
		BOOST_CHECK_EQUAL(15, *p2);
		*p1 = 16;
		BOOST_CHECK_EQUAL(16, *p2);
	}

	{
		int i = 17;
		akt::uniform_ptr<int> p1{ &i }; // take pointer to value
		akt::uniform_ptr<int> p2{ p1 };
		BOOST_CHECK_EQUAL(true, (bool)p2);
		BOOST_CHECK_EQUAL(17, *p2);
		*p1 = 18;
		BOOST_CHECK_EQUAL(18, *p2);
	}

	{
		akt::uniform_ptr<int> p1{ std::make_shared<int>(19) };
		akt::uniform_ptr<int> p2{ p1 };
		BOOST_CHECK_EQUAL(true, (bool)p2);
		BOOST_CHECK_EQUAL(19, *p2);
		*p1 = 20;
		BOOST_CHECK_EQUAL(20, *p2);
	}

	{
		akt::uniform_ptr<int> p1{ std::make_unique<int>(21) };
		akt::uniform_ptr<int> p2{ p1 };
		BOOST_CHECK_EQUAL(true, (bool)p2);
		BOOST_CHECK_EQUAL(21, *p2);
		*p1 = 22;
		BOOST_CHECK_EQUAL(22, *p2);
	}

	{
		akt::uniform_ptr<IntValue> p1;
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1;
		akt::uniform_ptr<IntNonCopyable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1;
		akt::uniform_ptr<IntNonMovable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntNonCopyable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntNonMovable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntValue> p1{ &i };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntValue> p1{ &i };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntNonCopyable> p1{ &i };
		akt::uniform_ptr<IntNonCopyable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntNonMovable> p1{ &i };
		akt::uniform_ptr<IntNonMovable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntNonCopyable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntNonMovable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntNonCopyable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntNonMovable> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_move_ctor)
{
	BOOST_CHECK_EQUAL(nullptr, akt::uniform_ptr<int>{ akt::uniform_ptr<int>{} }.get());
	BOOST_CHECK_EQUAL(false, (bool)akt::uniform_ptr<int>{ akt::uniform_ptr<int>{} });

	BOOST_CHECK_EQUAL(true, (bool)akt::uniform_ptr<int>{ akt::uniform_ptr<int>{23} });
	BOOST_CHECK_EQUAL(24, *akt::uniform_ptr<int>{ akt::uniform_ptr<int>{24} });

	int i1 = 25;
	BOOST_CHECK_EQUAL(true, (bool)akt::uniform_ptr<int>{ akt::uniform_ptr<int>{i1} });
	BOOST_CHECK_EQUAL(25, *akt::uniform_ptr<int>{ akt::uniform_ptr<int>{i1} });

	int i2 = 26;
	BOOST_CHECK_EQUAL(true, (bool)akt::uniform_ptr<int>{akt::uniform_ptr<int>{&i2}});
	BOOST_CHECK_EQUAL(26, *akt::uniform_ptr<int>{akt::uniform_ptr<int>{&i2}});

	BOOST_CHECK_EQUAL(true, (bool)akt::uniform_ptr<int>{akt::uniform_ptr<int>{std::make_shared<int>(27)}});
	BOOST_CHECK_EQUAL(28, *akt::uniform_ptr<int>{akt::uniform_ptr<int>{std::make_shared<int>(28)}});

	BOOST_CHECK_EQUAL(true, (bool)akt::uniform_ptr<int>{akt::uniform_ptr<int>{std::make_unique<int>(29)}});
	BOOST_CHECK_EQUAL(30, *akt::uniform_ptr<int>{akt::uniform_ptr<int>{std::make_unique<int>(30)}});

	BOOST_CHECK_EQUAL(false, (bool)akt::uniform_ptr<IntNonCopyable>{akt::uniform_ptr<IntNonCopyable>{}});

	BOOST_CHECK_EQUAL(false, (bool)akt::uniform_ptr<IntNonMovable>{ akt::uniform_ptr<IntNonMovable>{} });

	BOOST_CHECK_EQUAL(24, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ IntNonCopyable{ 24 } } }->getInt());

	BOOST_CHECK_EQUAL(24, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ IntNonMovable{ 24 } } }->getInt());

	BOOST_CHECK_EQUAL(24, akt::uniform_ptr<IntNonCopyable>{ akt::uniform_ptr<IntNonCopyable>{ IntNonCopyable{ 24 } } }->getInt());

	BOOST_CHECK_EQUAL(25, akt::uniform_ptr<IntNonMovable>{ akt::uniform_ptr<IntNonMovable>{ IntNonMovable{ 25 } } }->getInt());

	{
		IntNonCopyable i{ 25 };
		BOOST_CHECK_EQUAL(&i, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ &i } }.get());
	}

	{
		IntNonMovable i{ 25 };
		BOOST_CHECK_EQUAL(&i, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ &i } }.get());
	}

	{
		IntNonCopyable i{ 25 };
		BOOST_CHECK_EQUAL(&i, akt::uniform_ptr<IntNonCopyable>{ akt::uniform_ptr<IntNonCopyable>{ &i } }.get());
	}

	{
		IntNonMovable i{ 25 };
		BOOST_CHECK_EQUAL(&i, akt::uniform_ptr<IntNonMovable>{ akt::uniform_ptr<IntNonMovable>{ &i } }.get());
	}

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ std::make_shared<IntNonCopyable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ std::make_shared<IntNonMovable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntNonCopyable>{ akt::uniform_ptr<IntNonCopyable>{ std::make_shared<IntNonCopyable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntNonMovable>{ akt::uniform_ptr<IntNonMovable>{ std::make_shared<IntNonMovable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ std::make_unique<IntNonCopyable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntValue>{ std::make_unique<IntNonMovable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntNonCopyable>{ akt::uniform_ptr<IntNonCopyable>{ std::make_unique<IntNonCopyable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntNonMovable>{ akt::uniform_ptr<IntNonMovable>{ std::make_unique<IntNonMovable>(19) } }->getInt());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_copy_assign_op)
{
	akt::uniform_ptr<int> p0;
	akt::uniform_ptr<int> p1{ 31 };
	p1 = p0;
	BOOST_CHECK_EQUAL(p1.get(), p0.get());
	BOOST_CHECK_EQUAL(false, (bool)p1);

	akt::uniform_ptr<int> p2{ 32 }; // take value by moving
	akt::uniform_ptr<int> p3{ 33 };
	p3 = p2;
	BOOST_CHECK((bool)p3);
	BOOST_CHECK_EQUAL(32, *p3);
	*p2 = 34;
	BOOST_CHECK_EQUAL(34, *p3);

	int i1 = 35;
	akt::uniform_ptr<int> p4{ i1 }; // take value by copying
	akt::uniform_ptr<int> p5;
	p5 = p4;
	BOOST_CHECK((bool)p5);
	BOOST_CHECK_EQUAL(35, *p5);
	*p4 = 36;
	BOOST_CHECK_EQUAL(36, *p5);

	int i2 = 37;
	akt::uniform_ptr<int> p6{ &i2 }; // take pointer to value
	akt::uniform_ptr<int> p7;
	p7 = p6;
	BOOST_CHECK((bool)p7);
	BOOST_CHECK_EQUAL(37, *p7);
	*p6 = 38;
	BOOST_CHECK_EQUAL(38, *p7);

	akt::uniform_ptr<int> p8{ std::make_shared<int>(39) };
	akt::uniform_ptr<int> p9;
	p9 = p8;
	BOOST_CHECK((bool)p9);
	BOOST_CHECK_EQUAL(39, *p9);
	*p8 = 40;
	BOOST_CHECK_EQUAL(40, *p9);

	akt::uniform_ptr<int> p10{ std::make_unique<int>(41) };
	akt::uniform_ptr<int> p11;
	p11 = p10;
	BOOST_CHECK((bool)p11);
	BOOST_CHECK_EQUAL(41, *p11);
	*p10 = 42;
	BOOST_CHECK_EQUAL(42, *p11);

	{
		akt::uniform_ptr<IntValue> p1;
		akt::uniform_ptr<IntValue> p2{ IntNonCopyable{24} };
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1;
		akt::uniform_ptr<IntNonCopyable> p2{ IntNonCopyable{24} };
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1;
		akt::uniform_ptr<IntNonMovable> p2{ IntNonMovable{24} };
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntValue> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntValue> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntNonCopyable> p1{ &i };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntNonMovable> p1{ &i };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<int> p1{ 20 };
		p1 = p1;
		BOOST_CHECK_EQUAL(20, *p1);
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_move_assign_op)
{
	akt::uniform_ptr<int> p0;
	akt::uniform_ptr<int> p1{ 43 };
	p1 = std::move(p0);
	BOOST_CHECK_EQUAL(nullptr, p1.get());
	BOOST_CHECK_EQUAL(false, (bool)p1);

	akt::uniform_ptr<int> p2{ 44 }; // take value by moving
	akt::uniform_ptr<int> p3;
	p3 = std::move(p2);
	BOOST_CHECK((bool)p3);
	BOOST_CHECK_EQUAL(44, *p3);

	int i1 = 45;
	akt::uniform_ptr<int> p4{ i1 }; // take value by copying
	akt::uniform_ptr<int> p5;
	p5 = std::move(p4);
	BOOST_CHECK((bool)p5);
	BOOST_CHECK_EQUAL(45, *p5);

	int i2 = 46;
	akt::uniform_ptr<int> p6{ &i2 }; // take pointer to value
	akt::uniform_ptr<int> p7;
	p7 = std::move(p6);
	BOOST_CHECK((bool)p7);
	BOOST_CHECK_EQUAL(46, *p7);

	akt::uniform_ptr<int> p8{ std::make_shared<int>(47) };
	akt::uniform_ptr<int> p9;
	p9 = std::move(p8);
	BOOST_CHECK((bool)p9);
	BOOST_CHECK_EQUAL(47, *p9);

	akt::uniform_ptr<int> p10{ std::make_unique<int>(48) };
	akt::uniform_ptr<int> p11;
	p11 = std::move(p10);
	BOOST_CHECK((bool)p11);
	BOOST_CHECK_EQUAL(48, *p11);

	{
		akt::uniform_ptr<IntNonCopyable> p1;
		akt::uniform_ptr<IntNonCopyable> p2{ IntNonCopyable{ 24 } };
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(false, (bool)p2);
	}

	{
		akt::uniform_ptr<IntNonMovable> p1;
		akt::uniform_ptr<IntNonMovable> p2{IntNonMovable{ 25 }};
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(false, (bool)p2);
	}

	{
		akt::uniform_ptr<IntValue> p1{ IntNonCopyable{ 24 } };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(24, p2->getInt());
	}

	{
		akt::uniform_ptr<IntValue> p1{ IntNonMovable{ 24 } };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(24, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ IntNonCopyable{ 24 } };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(24, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ IntNonMovable{ 25 } };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(25, p2->getInt());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntValue> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(&i, p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntValue> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(&i, p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntNonCopyable> p1{ &i };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(&i, p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntNonMovable> p1{ &i };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(&i, p2.get());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntValue> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntNonCopyable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntNonMovable> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<int> p1{ 20 };
		p1 = std::move(p1);
		BOOST_CHECK_EQUAL(20, *p1);
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_copy_ctor_from_inherit)
{
	{
		akt::uniform_ptr<IntNonCopyable> p1;
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1;
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntNonCopyable> p1{ &i };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntNonMovable> p1{ &i };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2{ p1 };
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_move_ctor_from_inherit)
{
	BOOST_CHECK_EQUAL(false, (bool)akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonCopyable>{} });

	BOOST_CHECK_EQUAL(false, (bool)akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonMovable>{} });

	BOOST_CHECK_EQUAL(24, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonCopyable>{ IntNonCopyable{ 24 } } }->getInt());

	BOOST_CHECK_EQUAL(24, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonMovable>{ IntNonMovable{ 24 } } }->getInt());

	{
		IntNonCopyable i{ 25 };
		BOOST_CHECK_EQUAL(&i, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonCopyable>{ &i } }.get());
	}

	{
		IntNonMovable i{ 25 };
		BOOST_CHECK_EQUAL(&i, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonMovable>{ &i } }.get());
	}

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonCopyable>{ std::make_shared<IntNonCopyable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonMovable>{ std::make_shared<IntNonMovable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonCopyable>{ std::make_unique<IntNonCopyable>(19) } }->getInt());

	BOOST_CHECK_EQUAL(19, akt::uniform_ptr<IntValue>{ akt::uniform_ptr<IntNonMovable>{ std::make_unique<IntNonMovable>(19) } }->getInt());
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_copy_assign_op_from_inherit)
{
	{
		akt::uniform_ptr<IntNonCopyable> p1;
		akt::uniform_ptr<IntValue> p2{ IntNonCopyable{20} };
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1;
		akt::uniform_ptr<IntValue> p2{ IntNonMovable{21} };
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntNonCopyable> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntNonMovable> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = p1;
		BOOST_CHECK_EQUAL(p1.get(), p2.get());
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_move_assign_op_from_inherit)
{
	{
		akt::uniform_ptr<IntNonCopyable> p1;
		akt::uniform_ptr<IntValue> p2{ IntNonCopyable{20} };
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(false, (bool)p2);
	}

	{
		akt::uniform_ptr<IntNonMovable> p1;
		akt::uniform_ptr<IntValue> p2{ IntNonMovable{21} };
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(false, (bool)p2);
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ IntNonCopyable{24} };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(24, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ IntNonMovable{24} };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(24, p2->getInt());
	}

	{
		IntNonCopyable i{ 25 };
		akt::uniform_ptr<IntNonCopyable> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(&i, p2.get());
	}

	{
		IntNonMovable i{ 25 };
		akt::uniform_ptr<IntNonMovable> p1{ &i };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(&i, p2.get());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_shared<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_shared<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonCopyable> p1{ std::make_unique<IntNonCopyable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}

	{
		akt::uniform_ptr<IntNonMovable> p1{ std::make_unique<IntNonMovable>(19) };
		akt::uniform_ptr<IntValue> p2;
		p2 = std::move(p1);
		BOOST_CHECK_EQUAL(19, p2->getInt());
	}
}

BOOST_AUTO_TEST_CASE(test_uniform_ptr_bool_cast)
{
	BOOST_CHECK(false == (bool)akt::uniform_ptr<int>{});
	BOOST_CHECK(false == akt::uniform_ptr<int>{});
	BOOST_CHECK(!akt::uniform_ptr<int>{});
	BOOST_CHECK(false == (bool)akt::uniform_ptr<char>{nullptr});
	BOOST_CHECK(true == (bool)akt::uniform_ptr<bool>{false});
	BOOST_CHECK(true == akt::uniform_ptr<bool>{false});
	BOOST_CHECK(akt::uniform_ptr<bool>{false});
	BOOST_CHECK(false == (bool)akt::uniform_ptr<IntValue>{});
	BOOST_CHECK(false == (bool)akt::uniform_ptr<IntNonMovable>{});
	BOOST_CHECK(false == (bool)akt::uniform_ptr<IntNonCopyable>{nullptr});
}

