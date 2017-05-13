#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_PORT PORTB
#define LED PB0

void init()
{
	DDRB |= (1 << LED);
	
	// --------------------------
	
	OCR0A        = 0xFF;
	OCR0B 		 = 0xFF;                                          	// 0b 0 0 0 0 1 1 0 1   (prescaler 1/1024)
	TIMSK0       = (1 << OCIE0A);  				// work by interrupt
	TCCR0A  =  0x02; // CTC
	TCCR0B  =  0x05; // prescaler 1/1024
	
	// --------------------------
	
	sei();
}

char led_state = 1;
char timer_count = 0;

ISR (TIMER0_COMPA_vect)
{
	++timer_count;
	if (timer_count >= 20)
	{
		timer_count = 0;
		led_state ^= 1;
		if (led_state)
			LED_PORT |= (1 << LED);
		else
			LED_PORT &= ~(1 << LED);
	}
}


int main()
{
	init();
	
	while (1)
	{
		
	}
}
