#include <msp430.h>
#include "lcd.h"
// Delay function for producing delay in 0.1 ms increments
void delay(uint8_t t)
{
	uint8_t i;
	for(i=t; i > 0; i--)
		__delay_cycles(1600);
}

// Function to pulse EN pin after data is written
void pulseEN(void)
{
	LCD_OUT |= EN;
	delay(1);
	LCD_OUT &= ~EN;
	delay(1);
}

//Function to write data/command to LCD
void lcd_write(uint8_t value, uint8_t mode)
{
	if(mode == CMD)
		LCD_OUT &= ~RS;				// Set RS -> LOW for Command mode
	else
		LCD_OUT |= RS;				// Set RS -> HIGH for Data mode

	LCD_OUT = ((LCD_OUT & 0x0F) | (value & 0xF0));				// Write high nibble first
	pulseEN();
	delay(1);

	LCD_OUT = ((LCD_OUT & 0x0F) | ((value << 4) & 0xF0));		// Write low nibble next
	pulseEN();
	delay(1);
}

// Function to print a string on LCD
void lcd_print(char *s)
{
	while(*s)
	{
		lcd_write(*s, DATA);
		s++;
	}
}

void lcd_printNumber(unsigned long num)
{
	char buf[10];
	char *str = &buf[9];

	*str = '\0';

	do
	{
		unsigned long m = num;
		num /= 10;
		char c = (m - 10 * num) + '0';
		*--str = c;
	} while(num);

	lcd_print(str);
}

// Function to move cursor to desired position on LCD
void lcd_setCursor(uint8_t row, uint8_t col)
{
	const uint8_t row_offsets[] = { 0x00, 0x40};
	lcd_write(0x80 | (col + row_offsets[row]), CMD);
	delay(1);
}

void lcd_clear(void)
{
	lcd_write(0x01, CMD); 			// Clear screen
	delay(20);
}

// Initialize LCD
void lcd_init()
{
	P2SEL &= ~(BIT6+BIT7);
	LCD_DIR |= (D4+D5+D6+D7+RS+EN);
	LCD_OUT &= ~(D4+D5+D6+D7+RS+EN);

	delay(150);						// Wait for power up ( 15ms )
	lcd_write(0x33, CMD);			// Initialization Sequence 1
	delay(50);						// Wait ( 4.1 ms )
	lcd_write(0x32, CMD);			// Initialization Sequence 2
	delay(1);						// Wait ( 100 us )

	// All subsequent commands take 40 us to execute, except clear & cursor return (1.64 ms)

	lcd_write(0x28, CMD); 			// 4 bit mode, 2 line
	delay(1);

	lcd_write(0x0C, CMD); 			// Display ON, Cursor ON, Blink ON
	delay(1);

	lcd_clear();					// Clear the screen

	lcd_write(0x06, CMD); 			// Auto Increment Cursor
	delay(1);

	lcd_setCursor(0,0); 			// Goto Row 1 Column 1
}
