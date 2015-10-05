#ifndef _I2C_LCD_H_
#define _I2C_LCD_H_

#include "i2c_pcf8574.h"
#include <string.h>

/* LCD Specific configuration defines */

/* number of visible lines of the display */
#define LCD_LINES           4     

/* visibles characters per line of the display */
#define LCD_DISP_LENGTH    20     

/* 0: no wrap, 1: wrap at end of visibile */
#define LCD_WRAP_LINES      1     


#define LCD_LINE_LENGTH  0x40     // internal line length of the display   
#define LCD_START_LINE1  0x00     // DDRAM address of first char of line 1 
#define LCD_START_LINE2  0x40     // DDRAM address of first char of line 2 
#define LCD_START_LINE3  0x14     // DDRAM address of first char of line 3 
#define LCD_START_LINE4  0x54     // DDRAM address of first char of line 4 

/* display on/off, cursor on/off, blinking char at cursor position */
#define LCD_DISP_OFF             0x08   /* display off                            */
#define LCD_DISP_ON              0x0C   /* display on, cursor off                 */
#define LCD_DISP_ON_BLINK        0x0D   /* display on, cursor off, blink char     */
#define LCD_DISP_ON_CURSOR       0x0E   /* display on, cursor on                  */
#define LCD_DISP_ON_CURSOR_BLINK 0x0F   /* display on, cursor on, blink char      */


#define LCD_CLR               0      /* DB0: clear display                  */
#define LCD_HOME              1      /* DB1: return to home position        */
#define LCD_CGRAM             6      /* DB6: set CG RAM address             */
#define LCD_DDRAM             7      /* DB7: set DD RAM address             */


extern void lcd_init(uint8_t dispAttr);
extern void lcd_clrscr(void);
extern void lcd_home(void);
extern void lcd_putc(char c);
extern void lcd_puts(char *str);
extern void lcd_gotoxy(uint8_t x, uint8_t y);
extern void lcd_clrscr(void);
extern void lcd_backlight(uint8_t); //On: 1  , OFF: 0

#endif
