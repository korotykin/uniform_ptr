#include <iostream>

class StoredInt {
public:
	StoredInt(const int val): mValue(val)
	{}
public:
	int AddVal(const int val)
	{
		return mValue + val;
	}
private:
	int mValue = 0;
};

int main()
{
	StoredInt v(3);
    std::cout << v.AddVal(2) << std::endl;
	return 0;
}

