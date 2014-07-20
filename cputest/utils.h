#include <gctypes.h>
#include <string>

namespace CPUTest
{

inline void SetNonIEEEMode(bool enabled)
{
	if (enabled)
		asm("mtfsb1 29");
	else
		asm("mtfsb0 29");
}

inline void SetPairedSingleMode(bool enabled)
{
	const u32 PSE = 1 << (31 - 2);
	u32 hid2;
	asm("mfspr %0, 920" : "=r" (hid2));
	if (enabled)
		hid2 |= PSE;
	else
		hid2 &= ~PSE;
	asm("mtspr 920, %0" :: "r" (hid2));
}

inline void SetFlushPairedSingleStore(bool enabled)
{
	const u32 ST0 = 1 << (31 - 7);
	u32 hid4;
	asm("mfspr %0, 1011" : "=r" (hid4));
	if (enabled)
		hid4 |= ST0;
	else
		hid4 &= ~ST0;
	asm("mtspr 1011, %0" :: "r" (hid4));
}

} // namespace