#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "i2c_lcd.h"

extern void lcd_init(uint8_t dispAttr);
extern void lcd_clrscr(void);
extern void lcd_home(void);
extern void lcd_putc(char c);
extern void lcd_puts(char *str);
extern void lcd_gotoxy(uint8_t x, uint8_t y);
extern void lcd_clrscr(void);
extern void lcd_backlight(uint8_t); //On: 1  , OFF: 0



int main()
{
	lcd_init(LCD_DISP_ON_CURSOR_BLINK);	
	lcd_backlight(1);
	lcd_puts("this is a test message\nHow are you");	
	lcd_backlight(1);
	return 0;
}
