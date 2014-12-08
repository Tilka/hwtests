#include <gctypes.h>
#include <zlib.h>
#include <ogcsys.h>
#include "hwtests.h"

extern "C" void __SYS_ReadROM(void* buf, u32 len, u32 offset);
extern int client_socket;

static void extract_font(const char* name, u32 offset, u32 length)
{
	__attribute__((aligned(32))) static u8 buffer[0x4d000];
	__SYS_ReadROM(buffer, length, offset);

	u32 crc = crc32(0, buffer, length);
	printf("Connect via TCP to port 16784 to get the %s font (crc32: 0x%08x)\n", name, crc);

	network_init();
	int i = 0;
	const u32 chunk_size = 0x1000;
	while (length > chunk_size)
	{
		net_write(client_socket, &buffer[i], chunk_size);
		length -= chunk_size;
		i += chunk_size;
	}
	net_write(client_socket, &buffer[i], length);
	network_shutdown();
}

static GXRModeObj *rmode = NULL;
void * InitConsole() {
	VIDEO_Init();
	PAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	void *framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(framebuffer, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
}

int main()
{
	const u32 ansi_offset = 0x1fcf00;
	const u32 ansi_length = 0x3000;
	const u32 sjis_offset = 0x1aff00;
	const u32 sjis_length = 0x4d000;

	InitConsole();

	extract_font("ANSI", ansi_offset, ansi_length);
	extract_font("SJIS", sjis_offset, sjis_length);

	return 0;
}
