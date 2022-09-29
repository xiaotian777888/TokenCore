#include "zeroize.h"

#include <stdint.h>
#include <stdlib.h>

void zeroize(void* const buffer, size_t length)
{
#if defined(HAVE_MEMSET_S)
	if (memset_s(buffer, (rsize_t)length, 0, (rsize_t)length) != 0)
		abort();
#else
	size_t i;
	volatile uint8_t* vbuffer = (volatile uint8_t*)buffer;
	for (i = 0; i < length; i++)
		vbuffer[i] = 0;
#endif
}
