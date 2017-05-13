/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED 1				// PB1 - Индикаторная лампочка		(выход)	
#define BUTTON 2			// PB2 - Кнопочка					(вход)		
#define IRED 3				// PB3 - Инфракрасный светодиод		(выход)		
#define TSOP 4				// PB4 - TSOP - инфракрасный датчик (вход)

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // -> 1
//#define BIT_TIMER0_OVERFLOW_INTERRUPT TOIE0 // -> 1

#define BUTTONS_POLL_INTERVAL 20

void init()
{
	// настройка внешних устройств
	DDRB	   |=  (1 << LED) | (1 << IRED);			// Индикатор и мигающий диод - на выход
	DDRB       &= ~((1 << BUTTON) | (1 << TSOP));		// Кнопка и TSOP - на вход
	PORTB      &= ~((1 << LED) | (1 << IRED));			// По умолчанию лампочка не горит

	// настройка таймера
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// маска для прерывания от таймера 0 по совпадению счетчика TCNT0 таймера
	
	TCCR0       = 0x0A;									// b  0 0 0 0 1 0 1 0
														//      0     1        Режим CTC (сброс при совпадении)
														//              0 1 0  CS02..00 - Предделитель для таймера Т0 (равен 1/8) 
	
	OCR0        = 0x21;									// Максимальное значение регистра сравнения, равное 33d (до срабатывания таймера)
														//  16 000 000 / (8 * 33) ~= 60 606 ~= 60 kHz
	 
	sei();												// Разрешение глобальных прерываний
}

ISR (TIMER0_COMP_vect)
{
	if (PORTB & (1 << IRED))
		PORTB &= ~(1 << IRED);
	else
		PORTB |=  (1 << IRED);
}

int main(void)
{
	init();
	
	while (1)
	{
		if (!(PINB & (1 << TSOP))) // Если луч принимается, то на выходе TSOP выдает ноль
			PORTB |= (1 << LED);
		else
			PORTB &= ~(1 << LED);
	}
}