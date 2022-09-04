#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif


#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7


//#define F_CPU 14000000			    //14Mhz
#include <avr/io.h>					//Header file for AVR device specific I/O Definitions.
#include <avr/pgmspace.h>			//Header file for incorporating program space string utilities.
#include <avr/interrupt.h>			//Header file for incorportaing interrupt handling faclity (not used here).
#include <util/delay.h>				//Header file for incorporating delay routines.
#include "lcd2.h"					//Header file for the LCD Module driver.
#include "gpsb.c"
#include "lcd.h"

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define RATE 250					//Rate of scrolling.
void WaitMs(unsigned int ms);		//Declaration of delay routine used.

void usart_init();
unsigned int usart_getch();

unsigned char value,i,lati_value[15],lati_dir, longi_value[15], longi_dir ;
int main()
{
	
	unsigned char i;		//Max width of the LCD Display.
	
	DDRD = 0xFF;
	DDRC = 0xFF;
	Lcd4_Init();
	
	usart_init();
	
	while(1)
	{
		value=usart_getch();
		if(value=='$')
		{
			value=usart_getch();
			if(value=='G')
			{
				value=usart_getch();
				if(value=='P')
				{
					value=usart_getch();
					if(value=='G')
					{
						value=usart_getch();
						if(value=='G')
						{
							value=usart_getch();
							if(value=='A')
							{
								value=usart_getch();
								if(value==',')
								{
									value=usart_getch();
									while(value!=',')
									{
										value=usart_getch();
									}
									lati_value[0]=usart_getch();
									value=lati_value[0];
									for(i=1;value!=',';i++)
									{
										lati_value[i]=usart_getch();
										value=lati_value[i];
									}
									lati_dir=usart_getch();
									value=usart_getch();
									while(value!=',')
									{
										value=usart_getch();
									}
									longi_value[0]=usart_getch();
									value=longi_value[0];
									for(i=1;value!=',';i++)
									{
										longi_value[i]=usart_getch();
										value=longi_value[i];
									}
									longi_dir=usart_getch();
									
									_delay_ms(1);
									
									_delay_ms(1000);
									Lcd4_Set_Cursor(1,1);
									i=0;
									while(lati_value[i]!='\0')
									{
										Lcd4_Write_Char(lati_value[i]);
										i++;
									}
									Lcd4_Write_Char(lati_dir);
									
									_delay_ms(1000);
									Lcd4_Set_Cursor(2,1);
									i=0;
									while(longi_value[i]!='\0')
									{
										Lcd4_Write_Char(longi_value[i]);
										i++;
									}
									Lcd4_Write_Char(longi_dir);
									_delay_ms(1000);
								}
							}
						}
					}
				}
			}
		}
	}
	

	return 0;						//End of main function.
}



void WaitMs(unsigned int ms)		//Generates a delay of ms millisecond.
{
	unsigned int i;

	for(i=0;i<=ms;i++)
	{
		_delay_ms(1);
	}
}



void usart_init()
{
	
	UCSRB =  (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry

	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);   // Use 8-bit character sizes
	UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
}


unsigned int usart_getch()
{
	while (!(UCSRA & (1 << RXC))); // Do nothing until data have been recieved and is ready to be read from UDR
	return(UDR); // return the byte
}