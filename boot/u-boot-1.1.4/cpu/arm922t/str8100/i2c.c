// STAR str81XX i2c driver
// ref cpu/mpc824x/drivers/i2c/i2c.c

#include <common.h>

#undef I2CDBG

#define CONFIG_HARD_I2C
#ifdef CONFIG_HARD_I2C
#include <i2c.h> // include/i2c.h

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
}

int i2c_probe(uchar chip)
{
}

#endif
