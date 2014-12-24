#include <cstdlib>
#include <initializer_list>
#include <gctypes.h>
#include "hwtests.h"

static void CheckResult(s32 signed_input, s16 immediate, u32 output, bool carry)
{
	u32 input = static_cast<u32>(signed_input);
	u32 expected_output = immediate - input;
	bool expected_carry = immediate >= input;
	DO_TEST(carry == expected_carry,
	        "carry(0x%04x - 0x%08x):\n"
	        "     got 0x%08x\n"
	        "expected 0x%08x", immediate, input, carry, expected_carry);
	DO_TEST(output == expected_output,
	        "subfic: 0x%04x - 0x%08x:\n"
	        "     got 0x%08x\n"
	        "expected 0x%08x", immediate, input, output, expected_output);
}

template<s16 immediate>
inline static void DoTest(const s32 input)
{
	u32 output = input;
	u32 carry  = 0;
	asm(
		"subfic %[d_a], %[d_a], %[simm];"
		"addze %[ca], %[ca];"
		: [d_a] "+r" (output), [ca] "+r" (carry)
		: [simm] "I" (immediate)
		: "ca"
	);
	CheckResult(input, immediate, output, !!carry);
}

// Subtract immediate carrying
static void SubficTest()
{
	START_TEST();
	for (s32 i : {-32768, -32767, -2, -1, 0, 1, 2, 32766, 32767})
	{
		DoTest<-32768>(i);
		DoTest<-32767>(i);
		DoTest<-2>(i);
		DoTest<-1>(i);
		DoTest< 0>(i);
		DoTest<+1>(i);
		DoTest<+2>(i);
		DoTest<+32766>(i);
		DoTest<+32767>(i);
	}
	END_TEST();
}

int main()
{
	network_init();

	SubficTest();

	network_shutdown();

	return 0;
}
