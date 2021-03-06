#include "i2c_lcd.h"

static int device_handle = -1;
#define IA2C_LCD_ADDRESS 0x20

#define os_delay_us usleep
#define	os_strlen	strlen

#define LCD_DATA0_PIN   0
#define LCD_DATA1_PIN   1
#define LCD_DATA2_PIN   2
#define LCD_DATA3_PIN   3

#define LCD_RS_BIT      4
//#define LCD_RW_BIT      4
#define LCD_E_BIT       5
#define LCD_BL_BIT      6
#define LCD_BUTTON_BIT  7

//#define ENABLE_DEBUG        1

#ifdef ENABLE_DEBUG
    #define DBG printf
    #define DBG1
    #define DBG2
#else
    #define DBG(...)
    #define DBG1(...)
    #define DBG2
#endif

/* Internal Functions forward declarations */
uint8_t     set_rs_rw(uint8_t rs, uint8_t rw);
uint8_t     lcd_set_e();
uint8_t     lcd_clear_e();
void        lcd_toggle_e();
void        lcd_set_data_nibble(uint8_t nibble);
uint8_t     lcd_read_data_nibble();
void        lcd_command(uint8_t cmd);
void        lcd_write(int cmd_data, uint8_t byte);

#define _BV(bit)    (1<<bit)

static uint8_t max_x, max_y, curr_x, curr_y;

void lcd_init(uint8_t display)
{
    uint8_t dataport;

	device_handle = pcf8574_i2c_init(IA2C_LCD_ADDRESS);


    pcf8574_i2c_write_byte(device_handle, 0x00);
    max_y = LCD_LINES;
    max_x = LCD_DISP_LENGTH;

    os_delay_us(20*1000);

    set_rs_rw(0, 0);
    dataport = 0;
    dataport |= _BV(LCD_DATA1_PIN);
    dataport |= _BV(LCD_DATA0_PIN);
    lcd_set_data_nibble(dataport);
    lcd_toggle_e();
    os_delay_us(4992); /* delay, busy flag can't be checked here */

    /* repeat last command */
    lcd_toggle_e();
    os_delay_us(50);   /* delay, busy flag can't be checked here */

    /* repeat last command a third time */
    lcd_toggle_e();
    os_delay_us(64);   /* delay, busy flag can't be checked here */

    /* now configure for 4bit mode */
    dataport  = 0;
    dataport |= _BV(LCD_DATA1_PIN);
    lcd_set_data_nibble(dataport);
    lcd_toggle_e();
    os_delay_us(64);   /* some displays need this additional delay */

    lcd_command(0x28);
    lcd_command(0x08);
    lcd_command(0x06);
    lcd_command(0x01);
    lcd_command(0x80);
    lcd_command(display);
}




























void lcd_puts(char *str)
{
    uint8_t len, i;
    DBG("Current Position: %d %d\n", curr_x, curr_y);
    len = strlen(str);
    for (i=0;i<len;i++)
    {
        lcd_putc(*(str+i));
    }

    DBG("Current Position: %d %d\n", curr_x, curr_y);
}
void lcd_putc(char c)
{
    if (curr_x == max_x)
    {
        // wrap line
        curr_y++;
        curr_x = 0;
        lcd_gotoxy(curr_x, curr_y);
    }

    if (c != '\n')
    {
        lcd_write(1, c);
        curr_x++;
    }
    else
    {
        curr_y++;
        curr_x = 0;
        lcd_gotoxy(curr_x, curr_y);
    }
}


void lcd_clrscr(void)
{
    lcd_command(1<<LCD_CLR);
    curr_x = curr_y = 0;
}

void lcd_gotoxy(uint8_t x, uint8_t y)
{
    if (y == max_y)
    {
        y = 0;
    }

    if (x == max_x)
    {
        x = 0;
    }
    curr_x = x;
    curr_y = y;

#if LCD_LINES==1
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#endif
#if LCD_LINES==2
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
#endif
#if LCD_LINES==4
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else if ( y==1)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    else if ( y==2)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);
    else /* y==3 */
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif
    DBG("XY Current Position: %d %d \r\n", curr_x, curr_y);

}/* lcd_gotoxy */

//On: 1  , OFF: 0
void lcd_backlight(uint8_t state)
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);

    if (state)
        byte &= (~(1<<LCD_BL_BIT));
    else
        byte |= ((1<<LCD_BL_BIT));

    pcf8574_i2c_write_byte(device_handle, byte);
}

/* Internal Functions */
uint8_t set_rs_rw(uint8_t rs, uint8_t rw)
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);

    if (rs)
        byte |= ((1<<LCD_RS_BIT));
    else
        byte &= (~(1<<LCD_RS_BIT));

    if (rw == 1) printf("RW Should never be set to 1\r\n");
/*
    rw  = 0;
    if (rw)
        byte |= ((1<<LCD_RW_BIT));
    else
        byte &= (~(1<<LCD_RW_BIT));
*/
    pcf8574_i2c_write_byte(device_handle, byte);
	return 0;
}

void lcd_toggle_e()
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);
    byte |= ((1<<LCD_E_BIT));
    pcf8574_i2c_write_byte(device_handle, byte);
    os_delay_us(1); // Old Value  2 us
    byte &= (~(1<<LCD_E_BIT));
    pcf8574_i2c_write_byte(device_handle, byte);
}

void lcd_set_data_nibble(uint8_t nibble)
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);
    byte = byte & 0b11110000;    // Clear P0, P1, P2, P3
    byte |= (nibble & 0x0f);     // Or the the Data
    //os_printf(" SN: 0x%x ", byte);
    pcf8574_i2c_write_byte(device_handle, byte);
}

uint8_t lcd_read_data_nibble()
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);
    //os_printf("RN: 0x%x ", byte);
    byte = (byte & 0x0f);
    return byte;
}

uint8_t lcd_set_e()
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);
    byte |= ((1<<LCD_E_BIT));
    pcf8574_i2c_write_byte(device_handle, byte);
	return 0;
}

uint8_t lcd_clear_e()
{
    uint8_t byte;
    pcf8574_i2c_read_byte(device_handle, &byte);
    byte &= (~(1<<LCD_E_BIT));
    pcf8574_i2c_write_byte(device_handle, byte);
	return 0;
}

void lcd_waitbusy()
{
}

void lcd_command(uint8_t cmd)
{
    lcd_waitbusy();
    DBG1("\r\nCommand: 0x%x ", cmd);
    lcd_write(0, cmd);
    lcd_waitbusy();
    DBG1("\r\n");
}

// CMD: 0, Data 1
void lcd_write(int cmd_data, uint8_t byte)
{
    uint8_t high_nibble,low_nibble;         //Nibbles
/*
    if (cmd_data)
        os_printf("LCD: [0x%x] 0x%x", cmd_data, byte);
    else
        os_printf("LCD: [0x%x] %c", cmd_data, byte);
*/
    //os_printf("LCD: [0x%x] 0x%x  ---", cmd_data, byte);

    high_nibble = byte >>4;
    low_nibble  = (byte & 0x0F);

    if(cmd_data)
        set_rs_rw(1, 0);
    else
        set_rs_rw(0, 0);

    os_delay_us(1);

    lcd_set_data_nibble(high_nibble);
    lcd_toggle_e();
    os_delay_us(1);
    lcd_set_data_nibble(low_nibble);
    lcd_toggle_e();

    // Most LCD operations take 37us
    os_delay_us(40);
}

