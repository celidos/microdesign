/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// PA0 (ADC0 - нулевой канал АЦП) - движок потенциометра, который дает сигнал
#define RESIST_PORT PORTA	// Регистр данных порта А
#define RESIST_DDR DDRA		// Реистр направления порта А (на вход или на выход)
#define RESIST_PIN PINA		// Пины
#define RESIST 0			// Потенциометр на нулевой ножке

// PB1 - лампочка (выход)
#define LED_PORT PORTB		// Регистр данных
#define LED_DDR DDRB		// Направление данных (побитно настраивает каждый пин на вход или на выход)
#define LED_PIN PINB		// Пины
#define LED 1				// Номер пина

// PB2 - кнопочка (вход)
#define BUTTON_PORT PORTB
#define BUTTON_DDR DDRB
#define BUTTON_PIN PINB
#define BUTTON 2

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // = 1

#define BUTTONS_POLL_INTERVAL 20

int  timer_count      = 0;
int  max_timer_count  = 61;

char led_state        = 0;

void init()
{
	// настройка внешних устройств
	RESIST_DDR  = 0;              // Все пины порта, на котором висит потенциометр, настраиваем как входы
	
	LED_DDR    |= (1 << LED);     // пин светодиода настраиваем как выход
	LED_PORT   &= ~(1 << LED);    // По дефолту светодиод выключаем (в регистр данных ставим 0)
	BUTTON_DDR &= ~(1 << BUTTON); // пин кнопки принудительно ставим в 0 (т.е. кнопка как вход)
	
	// настройа таймера
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// маска для прерывания от таймера 0 по совпадению OCR0 и счетчика TCNT0 таймера
	
	TCCR0       = 0x0D;//(1 << WGM01) | (1 << CS02) | (1 << CS00); // 0b 0 0 0 0 1 1 0 1   (предделитель 1/1024)
	OCR0        = 0xFF;									// Максимальное значение регистра сравнения (до срабатывания таймера)
	
	sei();												// SREG		= (1 << 7);
	
	// настраиваем АЦП
	ADMUX       = 0x60;			// b 0 1 1 0 0 0 0 0
	                            //   0 1              Источник опорного напряжения - AVcc = 5V
								//       1            Выравнивание результата влево  ( DDDDDDDD DDxxxxxx )
								//         0 0 0 0 0  Выбор выходного канала (ADC0, обычный, без усиления)
	
	SFIOR      &= 0x1F;         // b 0 0 0 ? ? ? ? ? 
								//   0 0 0            Режим непрерывного преобразования

	ADCSRA      = 0xE7;         // b 1 1 1 0 0 1 1 1 
								//   1                Включает блок АЦП
								//     1              Запускает преобразование ("единичное" преобразование)
								//       1            Устанавливает режим работы АЦП в соответствии с регистром SFIOR (непрерывного преобразования)
								//         0          Флаг готовности результата
								//           0        Запрет прерывания по готовности результата (здесь не нужно)
								//             1 1 1  Задают предделитель (здесь максимальный: 1/128)
	
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
		if (ADCSRA & (1 << ADIF))
		{
			char t = ADCL;
			     t = ADCH;
			
			max_timer_count = 2 + (t >> 1);
		}
		/*cur = (BUTTON_PIN & (1 << BUTTON));
		if (cur && !last_button_state)			// Если кнопка была нажата
		{
			flashing_enabled ^= 1;				//  переключаем состояние
			
		}
		_delay_ms(BUTTONS_POLL_INTERVAL);*/
	}
}