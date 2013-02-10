#include "common-test.h"

CommonTest::CommonTest()
{
	ctx = new Context();
}

CommonTest::~CommonTest()
{
	delete ctx;
}
