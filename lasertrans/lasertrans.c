/* Atmega 8535 */

#define F_CPU 16000000

#define LED 1				// PB1 - LED pbit number (output)
#define BUTTON 2			// PB2 - button pin number (input)

#define LASER 3 			// PB3 - laser

#define BUTTONS_POLL_INTERVAL 10

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // = 1

#define MAXDATA 64

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char data[MAXDATA];
char ndex = 0;

char max_timer_count_div2;
char max_timer_count;

char recievingSomething = 0;
char translatingSomething = 0;

char timer_count = 0;


char trans_byte = 0;
char trans_bit  = 0;

void run_timer()
{
	OCR0        = 0xFF;
	TCCR0       = 0x0D;                                             	// 0b 0 0 0 0 1 1 0 1   (prescaler 1/1024)
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);  				// work by interrupt
	
	timer_count = 0;
	trans_byte  = 0;
	trans_bit   = 0;
	
	sei();
}

void stop_timer()
{
	TCCR0       =  0x00;                                          		
	TIMSK       &= ~(1 << BIT_TIMER0_COMPARE_INTERRUPT);  
	
	cli();				
}

void init()
{
	// ----------------- peripheral devices (buttons, leds etc) ---------
	
	DDRB	   |=  (1 << LED) | (1 << LASER);	// Лампочка - на выход (кнопка на вход по умолчанию)
	DDRB       &= ~(1 << BUTTON);
	PORTB       =  0;		// As default lamp is not enabled
	
	// ----------------- serial port initialization --------------------
	
	UCSRA       =  0x00;	// b 0 0 0 0 0 0 0 0
							//   0                Флаг - прием завершен (если стоит 1)
							//     0              Флаг - передача заврешена (если стоит 1)
							//       0            0 показывает, что буфер пустой
							//         0 0 0      Ошибки не обрабатываем
							//               0    Двойная скорость не нужна
							//                 0  Наркоманский режим, который не нужен
	
	UCSRB		=  0x98;	//
	UCSRC		=  0x86;    //
	UBRRH		=  0x00;    // Старший байт скорости USART - скорость 9600 влезает в один байт
	UBRRL		=  0x67;    // 103d - задает скорость 9600 бит/с
	
	// ---- Global variables initialization ----------------------------
	
	ndex = 0;
	
	translatingSomething = 0;
	recievingSomething   = 0;
	
	// ----- Timer initialization --------------------------------------

	max_timer_count_div2 = 60;
	max_timer_count      = 2 * max_timer_count_div2;
	OCR0                 = 0xFF;
	
	// ----- Confirm interrupts ----------------------------------------
	
	//sei();
}

char led_state = 0;
char sizeofdata = 0;
long int overtime_count = 0;
char wasdata = 0;

// ---------------- INTERRUPT TIMER ------------------------------------
ISR (TIMER0_COMP_vect)
{
	++timer_count;
	if (timer_count >= max_timer_count)
	{
		timer_count = 0;
		led_state ^= 1;
		if (led_state)
			PORTB |= (1 << LED);
		else
			PORTB &= ~(1 << LED);
		
		
		if (trans_bit == 0 || trans_bit == 9) 	// start and stop bits
		{
			PORTB |= (1 << LASER);
		}
		else if (trans_bit == 10)
		{
			PORTB &= ~(1 << LASER);
		}
		else if (trans_bit == 11)
		{
			PORTB &= ~(1 << LASER);
			trans_bit = 0;
			++trans_byte;
			if (trans_byte >= sizeofdata) 		// transmission finito
			{
				stop_timer();
				translatingSomething = 0;
				sizeofdata = 0;
				ndex = 0;
				wasdata = 0;
			}
		}
		else
		{
			char result  = ((data[trans_byte] >> (trans_bit - 1)) & 1);
			if (result)
				PORTB |=  (1 << LASER);
			else
				PORTB &= ~(1 << LASER);
		}
		++trans_bit;
	}
}

// ---------------- INTERRUPT SERIAL -----------------------------------

/*ISR (USART_RX_vect)
{
	if (!translatingSomething)
	{
		if (recievingSomething) // recieving now, and not translating
		{
			data[ndex] = UDR;
			if (ndex + 1 >= data[0])
			{
				recievingSomething = 0;
				translatingSomething = 1;
				
				transmit_data(); 
				
				run_timer();
			}
			else
			{
				ndex = (ndex + 1) & (MAXDATA - 1);
			}
		}
		else // if nothing recieving now
		{
			recievingSomething = 1;
			data[0] = UDR;
			ndex = 1;
		}
	}
} */

void recieve_data()
{
	if ((overtime_count <= 1500) && !wasdata)
	{
		if (UCSRA & (1 << RXC))
		{
			data[ndex] = UDR;
			++ndex;
			overtime_count = 0;
		}
		else
			++overtime_count;
	}
	else
	{
		overtime_count = 0;
		if (ndex != 0)
		{
			sizeofdata = ndex;
			ndex = 0;
			wasdata = 1;
			run_timer();
		}
	}
}

char j = 0;

void transmit_data()
{
	if (wasdata && (UCSRA & (1 << UDRE)))
	{
		if (j < sizeofdata)
		{
			UDR = data[j];
			++j;
		}
		else
		{
			wasdata = 0;
			j = 0;
		}
	}
}

int main(void)
{
	init();	
	
	while (1)
	{
		recieve_data();
		transmit_data();
	}
}
