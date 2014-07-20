#include <gctypes.h>
#include <paired.h>
#include <wiiuse/wpad.h>
#include "test.h"
#include "utils.h"

typedef float v2sf __attribute__ ((vector_size (8)));

static void FselTest()
{
	START_TEST();
	CPUTest::SetNonIEEEMode(false);
	CPUTest::SetPairedSingleMode(true);
	CPUTest::SetFlushPairedSingleStore(false);
	// Deliberately use often-mangled values for b and c.
	const u32 B = 0x000bbbbb; // subnormal
	const u32 C = 0xffbccccc; // SNaN
	u32 __attribute__ ((aligned (8))) values[][2][2] = {
		{{0x00000000, 0x80000000}, {C, C}}, // +0, -0
		{{0x00000001, 0x80000001}, {C, B}}, // smallest positive subnormal, largest negative subnormal
		{{0x007fffff, 0x807fffff}, {C, B}}, // largest positive subnormal, smallest negative subnormal
		{{0x7f800000, 0xff800000}, {C, B}}, // +infinity, -infinity
		{{0xffffffff, 0xffbfffff}, {B, B}}, // QNaN, SNaN
	};
	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++)
	{
		v2sf a, b, c, d;
		u32 result[2], expected[2];

		a = paired_lx(0, (float*)values[i][0]);
		b = paired_lx(0, (float*)values[4][1]);
		c = paired_lx(0, (float*)values[0][1]);
		d = paired_sel(a, c, b);
		paired_stx(d, 0, (float*)result);
		expected[0] = values[i][1][0];
		expected[1] = values[i][1][1];
		DO_TEST(result[0] == expected[0], "ps_sel(0x%08x):\n"
		                                  "     got 0x%08x\n"
		                                  "expected 0x%08x", *(u32*)&a[0], result[0], expected[0]);
		DO_TEST(result[1] == expected[1], "ps_sel(0x%08x):\n"
		                                  "     got 0x%08x\n"
		                                  "expected 0x%08x", *(u32*)&a[1], result[1], expected[1]);
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
