#include <common.h>

#ifdef CONFIG_STAR_GSW

int eth_init(bd_t *bis)
{
	return 0;
}

s32 eth_send(volatile void *packet, s32 length)
{
	return 0;
}

s32 eth_rx(void)
{
	return 0;
}

void eth_halt(void)
{
	return;
}

#endif

