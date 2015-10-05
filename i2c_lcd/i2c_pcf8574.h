#ifndef _PCF8754_H_
#define _PCF8754_H_

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

typedef unsigned char uint8_t;
int 	pcf8574_i2c_init(uint8_t device_address);
uint8_t pcf8574_i2c_read_byte(int device,uint8_t *byte);
uint8_t pcf8574_i2c_write_byte(int device, uint8_t byte);

#endif

