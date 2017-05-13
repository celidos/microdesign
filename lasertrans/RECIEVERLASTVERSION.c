/* ATmega 2560, arduino */

#define LED 5
#define DIODE_PIN 0
#define DIODE_PORT PIND

#define DELAY_TIMEOUT 3

#define MAX_TIMER 60

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char timer_count = 0;
char max_timer_count_div2;
char max_timer_count;

char led_state = 1;

char recieving_byte = 0; // 0..10
char ok = 1;

char offtimer = 0;
char data[64];
char ndex = 0;
char curbyte = 0;
char j = 0;
char wasdata = 0;

void enable_int()
{
	EIMSK  =  0x01;			// b 0 0 0 0 0 0 0 1
							//   0 0 0 0 0 0 0   - INT7 - INT1
							//                 1 - Enable INT0
							
	EICRA  =  0x02;			// b 0 0 0 0 0 0 1 0
							//               1 0 - creating interrupt not by falling (000000000111111111)
}

void disable_int()
{
	EIMSK  =  0x00;					
}

void run_timer()
{
	OCR0A   =  0xFF; // 8-bit counter
	OCR0B   =  0x00;
	
	TIMSK0 |= (1 << OCIE0A); // compare interrupt
	
	TCCR0A  =  0x02; // CTC
	TCCR0B  =  0x05; // prescaler 1/1024
	
	timer_count = 0;
}

void stop_timer()
{
	TCCR0A       =  0x00;                                          		
	TIMSK0       =  0x00;  
	
	cli();	
}

ISR (TIMER0_COMPA_vect)
{
	++timer_count;
	if (recieving_byte == 0)
	{
		if (timer_count >= max_timer_count_div2)
		{
			led_state ^= 1;
			if (led_state)
				PORTC = 0xFF;
			else
				PORTC = 0x00;
			timer_count = 0;
			++recieving_byte;
			if ((DIODE_PORT & (1 << DIODE_PIN)))
				ok = 0;
		}
	}
	else 
	{
		if (timer_count >= max_timer_count)
		{
			timer_count = 0;
			++recieving_byte;
			
			led_state ^= 1;
			if (led_state)
				PORTC = 0xFF;
			else
				PORTC = 0x00;

			if (recieving_byte >= 10)
			{					
				stop_timer();
				
				j = 0;
				wasdata = 1;
			}
			else if (recieving_byte > 1)
			{
				//curbyte = (curbyte << 1) + ((DIODE_PORT & (1 << DIODE_PIN)) != 0);
				
				//curbyte = curbyte | (((DIODE_PORT & (1 << DIODE_PIN)) != 0) << (recieving_byte - 2));
				//curbyte = 0x35;
				if ((DIODE_PORT & (1 << DIODE_PIN)) == 0)
					curbyte |= (1 << recieving_byte - 2);
					//++curbyte;
			}	
		}
	}
}

ISR (INT0_vect)
{
	PORTC = 0xFF;
	disable_int();
	
	ok = 1;
	timer_count = 0;
	curbyte = 0;
	offtimer = 0;
	recieving_byte = 0;
	
	
	run_timer();
}

void init()
{
	// -------------------- external interrupts ------------------------

	enable_int();
	
	// ---------------------- peripheral -------------------------------
	
	DDRC  |= (1 << LED);		// led out
	PORTC = 0x00;		 		// disabled as default
	
	
	// ---------------------- serial port ------------------------------
	
	UCSR0A       =  0x00;	// b 0 0 0 0 0 0 0 0
							//   0                Флаг - прием завершен (если стоит 1)
							//     0              Флаг - передача заврешена (если стоит 1)
							//       0            0 показывает, что буфер пустой
							//         0 0 0      Ошибки не обрабатываем
							//               0    Двойная скорость не нужна
							//                 0  Наркоманский режим, который не нужен
	
	UCSR0B		=  0x98;	//
	UCSR0C		=  0x86;    //
	UBRR0H		=  0x00;    // Старший байт скорости USART - скорость 9600 влезает в один байт
	UBRR0L		=  0x67;    // 103d - задает скорость 9600 бит/с
	
	// ----------------------- also ------------------------------------
		
	max_timer_count_div2 = MAX_TIMER;
	max_timer_count = max_timer_count_div2 * 2;
	
	wasdata = 0;
	ndex = 0;
	
	// --------------------- enable interrupts -------------------------
	
	sei();
}

void transmit_data()
{
	if (wasdata && (UCSR0A & (1 << UDRE0)))
	{
		if (j < ndex)
		{
			UDR0 = data[j];
			++j;
		}
		else
		{
			wasdata = 0;
			j = 0;
			curbyte = recieving_byte = 0;
		}
	}
}

short int timeout_c = 0;

int main()
{
	init();
	
	while (1)
	{
		transmit_data();
	}
}
