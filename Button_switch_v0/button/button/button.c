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
	
	char led_state = 0;
	char last_button_state = 0;
	char cur;

	while (1)
	{
		cur = (BUTTON_PIN & (1 << BUTTON));
		if (cur && !last_button_state) // Если кнопка нажата
		{
			led_state = 1 - led_state;
		}
		
		if (led_state)	
			LED_PORT |= (1 << LED);		// включаем светодиод (инверсия на микросхеме)
		else
			LED_PORT &= ~(1 << LED);	// Иначе выключаем
		
		last_button_state = cur;
		_delay_ms(BUTTONS_POLL_INTERVAL);
	}
}