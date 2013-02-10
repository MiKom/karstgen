#include <algorithm>

#include "context.h"
#include "gtest/gtest.h"

class CommonTest : public testing::Test
{
protected:
	
	Context *ctx;

	CommonTest();
	virtual ~CommonTest();

	template<class T>
	static testing::AssertionResult arrays_equal(T *array1, T *array2, size_t size) {
		if(std::equal(array1, array1+size, array2)) {
			return ::testing::AssertionSuccess();
		} else {
			return ::testing::AssertionFailure() << "Arrays differ";
		}
	}
};
