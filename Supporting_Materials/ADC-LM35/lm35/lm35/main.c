/*
 * lm35.c
 *
 * Created: 5/29/2021 7:29:21 PM
 * Author : Tareq
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRB = 0xFF;
	
	/*
	Vref = 2.56 -> 11
	ADLAR = right adjusted -> 0
	Source = ADC0 -> 00000
	*/
	ADMUX = 0b11000000;
	
	/*
	ADEN -> 1
	Prescaler 2 factor -> 000
	*/
	ADCSRA = 0b10000000;
	
	
    while (1) 
    {
		ADCSRA |= (1 << ADSC);
		while(ADCSRA & (1 << ADSC)) {;}
		
		// 2.56 * 1000 / (1024 * 10) = 0.25 = 1/4
		float temp = ADC / 4.0;
		
		if(temp > 20) PORTB = 0xFF;
		else PORTB = 0x00;
    }
}

