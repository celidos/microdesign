/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

// PB1 - лампочка (выход)
#define LED_PORT PORTB		// Регистр данных
#define LED_DDR DDRB		// Направление данных (побитно настраивает каждый пин на вход или на выход)
#define LED_PIN PINB		// ?
#define LED 1				// Номер пина

// PB2 - кнопочка (вход)
#define BUTTON_PORT PORTB
#define BUTTON_DDR DDRB		
#define BUTTON_PIN PINB
#define BUTTON 2

#define BUTTONS_POLL_INTERVAL 10

int main(void)
{
	LED_DDR    |= (1 << LED);     // пин светодиода настраиваем как выход
	LED_PORT   &= ~(1 << LED);    // По дефолту выключаем
	BUTTON_DDR &= ~(1 << BUTTON); // пин кнопки принудительно ставим в 0

	while (1)
	{
		if (!(BUTTON_PIN & (1 << BUTTON))) // Если кнопка нажата
		{
			LED_PORT |= (1 << LED);		// включаем светодиод (инверсия на микросхеме)
		}
		else
		{
			LED_PORT &= ~(1 << LED);	// Иначе выключаем
		}
		_delay_ms(BUTTONS_POLL_INTERVAL);
	}
}
