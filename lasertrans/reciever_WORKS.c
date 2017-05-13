/* ATmega 2560, arduino */

#define F_CPU 16000000

#define LED 5

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char timer_count = 0;
char max_timer_count = 30;

void run_timer()
{
	OCR0A   =  0xFF; // 8-bit counter
	OCR0B   =  0x00;
	
	TIMSK0 |= (1 << OCIE0A); // compare interrupt
	
	TCCR0A  =  0x02; // CTC
	TCCR0B  =  0x05; // prescaler 1/1024
	
	timer_count = 0;
}

char led_state = 0;

ISR (TIMER0_COMPA_vect)
{
	++timer_count;
	if (timer_count >= max_timer_count)
	{
		timer_count = 0;
		
		led_state ^= 1;
		if (led_state)
			PORTC |= (1 << LED);
		else
			PORTC &= ~(1 << LED);
	}
}

void init()
{
	// -------------------- external interrupts ------------------------
	
	EIMSK  =  0x01;			// b 0 0 0 0 0 0 0 1
							//   0 0 0 0 0 0 0   - INT7 - INT1
							//                 1 - Enable INT0
							
	EICRA  =  0x02;			// b 0 0 0 0 0 0 1 0
							//               1 0 - creating interrupt by falling (11111111100000000)
	
	// ---------------------- peripheral -------------------------------
	
	DDRC |= (1 << LED);		// led out
	PORTC |= (1 << LED); 		// enabled as default
	
	sei();
}

int main()
{
	init();
	run_timer();
	
	while (1)
	{
		
	}
}
