#include <gctypes.h>
#include <wiiuse/wpad.h>
#include "test.h"

static void FselTest()
{
	START_TEST();
	// Deliberately use often-mangled values for b and c.
	const u64 b = 0x0000000000000001; // subnormal
	const u64 c = 0xfff7ffffffffffff; // SNaN
	u64 values[][2] = {
		// a (input)         d (expected result)
		{0x0000000000000000, c}, // +0
		{0x8000000000000000, c}, // -0
		{0x0000000000000001, c}, // smallest positive subnormal
		{0x8000000000000001, b}, // largest negative subnormal
		{0x000fffffffffffff, c}, // largest positive subnormal
		{0x800fffffffffffff, b}, // smallest negative subnormal
		{0x7ff0000000000000, c}, // +infinity
		{0xfff0000000000000, b}, // -infinity
		{0xffffffffffffffff, b}, // a QNaN
		{0xfff7ffffffffffff, b}, // a SNaN
	};
	// Enable non-IEEE mode.
	asm("mtfsb1 29");
	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++)
	{
		u64 a = values[i][0];
		u64 expected = values[i][1];
		u64 d = 0;
		asm("fsel %0, %1, %2, %3" : "=f" (d) : "f" (a), "f" (c), "f" (b));
		DO_TEST(d == expected, "fsel(0x%016llx):\n"
		                       "     got 0x%016llx\n"
		                       "expected 0x%016llx", a, d, expected);
	}
	END_TEST();
}

int main()
{
	network_init();
	WPAD_Init();

	FselTest();

	network_printf("Shutting down...\n");
	network_shutdown();

	return 0;
}
