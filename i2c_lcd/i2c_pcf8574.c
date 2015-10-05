#include "i2c_pcf8574.h"

#define I2C_BUS	"/dev/i2c-1"

int pcf8574_i2c_init(uint8_t device_address)
{
	int deviceHandle = 0;
	if ((deviceHandle = open(I2C_BUS, O_RDWR)) < 0) 
	{
        printf("Error: Couldn't open device! %d\n", deviceHandle);
        return -1;
    }

    if (ioctl(deviceHandle, I2C_SLAVE, device_address) < 0) 
	{
        printf("Error: Couldn't find device on address!\n");
        return -1;
    }
	
	return (deviceHandle);
}

uint8_t pcf8574_i2c_read_byte(int device, uint8_t *byte)
{
	int nRet = 0;

	nRet = read(device, byte, 1);

	return nRet;
}

uint8_t pcf8574_i2c_write_byte(int device, uint8_t byte)
{
	int nRet = 0;

	nRet = write(device, &byte, 1);

	return nRet;
}
