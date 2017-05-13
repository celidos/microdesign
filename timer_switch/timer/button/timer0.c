/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // = 1

#define BUTTONS_POLL_INTERVAL 20

int  timer_count      = 0;
char max_timer_count  = 61;

char led_state        = 0;
//char flashing_enabled = 0;

void init()
{
	LED_DDR    |= (1 << LED);     // пин светодиода настраиваем как выход
	LED_PORT   &= ~(1 << LED);    // По дефолту выключаем
	BUTTON_DDR &= ~(1 << BUTTON); // пин кнопки принудительно ставим в 0
	
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// маска для прерывания от таймера 0 по свопадению OCR0 и счетчика TCNT0 таймера
	
	TCCR0       = 0x0D;//(1 << WGM01) | (1 << CS02) | (1 << CS00); // 0b 0 0 0 0 1 1 0 1   (предделитель 1/1024)
	OCR0        = 0xFF;									// Максимальное значение регистра сравнения (до срабатывания таймера)
	
	sei();												// SREG		= (1 << 7);
	
	timer_count	= 0;									// обнуляем счетчик
	led_state   = 0;									// изначально выключена
}

ISR (TIMER0_COMP_vect)
{
	++timer_count;
	if (timer_count >= max_timer_count)
	{
		led_state ^= 1;
		timer_count = 0;
		
		if (led_state)
			LED_PORT |= (1 << LED);			// включаем светодиод, когда нужно
		else
			LED_PORT &= ~(1 << LED);		// Иначе выключаем
	}
}

int main(void)
{
	init();
	
	//char last_button_state = 0;
	//char cur;
	
	while (1)
	{
		if (BUTTON_PIN & (1 << BUTTON))
			max_timer_count = 61;			// включаем светодиод, когда нужно
		else
			max_timer_count = 6;		// Иначе выключаем
		/*cur = (BUTTON_PIN & (1 << BUTTON));
		if (cur && !last_button_state)			// Если кнопка была нажата
		{
			flashing_enabled ^= 1;				//  переключаем состояние
			
		}
		_delay_ms(BUTTONS_POLL_INTERVAL);*/
	}
}