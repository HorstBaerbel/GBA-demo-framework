#include "output.h"

#ifdef TARGET_PC
#include <cstdlib>
#include <iostream>

void print(const char *s)
{
	std::cout << s;
}
#else
#include <cstring>
#include <cstdint>

#define MGBA_REG_DEBUG_ENABLE (volatile uint16_t *)0x4FFF780
#define MGBA_REG_DEBUG_FLAGS (volatile uint16_t *)0x4FFF700
#define MGBA_REG_DEBUG_STRING (char *)0x4FFF600

// mGBA debug output through registers
void print(const char *s)
{
	*MGBA_REG_DEBUG_ENABLE = 0xC0DE;
	strcpy(MGBA_REG_DEBUG_STRING, s);
	*MGBA_REG_DEBUG_FLAGS = 4 | 0x100;
	//*MGBA_REG_DEBUG_ENABLE = 0;
}
/*
	// VisualBoyAdvance debug output through SWI 0xff
	#if defined(__thumb__) || defined(_M_ARMT)
		void print(const char *s) // THUMB code
		{
			asm volatile("mov r0, %0;"
						"swi 0xff;"
						: // no ouput
						: "r" (s)
						: "r0");
		}
	#else
		void print(const char *s) // ARM code
		{
			asm volatile("mov r0, %0;"
						"swi 0xff0000;"
						: // no ouput
						: "r" (s)
						: "r0");
		}
	#endif
*/
#endif
