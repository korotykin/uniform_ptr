#include <iostream>
#include <variant>

class StoredInt {
public:
	explicit StoredInt(const int val): mValue(val) {}
	explicit StoredInt(int * const val) : mValue(val) {}
public:
	int AddVal(const int val) const
	{
		return *get() + val;
	}
private:
	std::variant<int, int *> mValue;
	const int * get() const
	{
		switch (mValue.index()) {
		case 0:
			return &std::get<0>(mValue);
			break;
		case 1:
			return std::get<1>(mValue);
			break;
		}
	}
};

int main()
{
	StoredInt v1{ 3 };
	int b = 4;
	StoredInt v2{ &b };
    std::cout << "::: " << v1.AddVal(2) << " ::: " << v2.AddVal(7) << std::endl;
	return 0;
}

