/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // -> 1
//#define BIT_TIMER0_OVERFLOW_INTERRUPT TOIE0 // -> 1

char reg_switcher = 1;

void init()
{
	// настройка внешних устройств
	DDRB	   |=  (1 << 1);							// Диод - на выход
	DDRD       |=  (1 << 7);                            //  PD7 на выход
	PORTB      &= ~(1 << 1);							// По умолчанию лампочка не горит

	// настройка таймера
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// маска для прерывания от таймера 0 по совпадению счетчика TCNT0 таймера
	
	TCCR0       = 0x0A;									// b  0 0 0 0 1 0 1 0
														//      0     1        Режим CTC (сброс при совпадении)
														//              0 1 0  CS02..00 - Предделитель для таймера Т0 (равен 1/8) 
	
	OCR0        = 0x40;									// Максимальное значение регистра сравнения, равное 33d (до срабатывания таймера)
														//  16 000 000 / (8 * 33) ~= 60 606 ~= 60 kHz
	
	// настраиваем АЦП
	ADMUX       = 0x60;			// b 0 1 1 0 0 0 0 0
	//   0 1              Источник опорного напряжения - AVcc = 5V
	//       1            Выравнивание результата влево  ( DDDDDDDD DDxxxxxx )
	//         0 0 0 0 0  Выбор входного канала (ADC0, обычный, без усиления)
	
	SFIOR      &= 0x1F;         // b 0 0 0 ? ? ? ? ?
	//   0 0 0            Режим непрерывного преобразования

	ADCSRA      = 0xE1;         // b 1 1 1 0 0 1 1 1
	//   1                Включает блок АЦП
	//     1              Запускает преобразование ("единичное" преобразование)
	//       1            Устанавливает режим работы АЦП в соответствии с регистром SFIOR (непрерывного преобразования)
	//         0          Флаг готовности результата
	//           0        Запрет прерывания по готовности результата (здесь не нужно)
	//             1 1 1  Задают предделитель (здесь максимальный: 1/128)
	
	sei();												// Разрешение глобальных прерываний
}

int time_count = 0;
int max_timer = 128;

ISR (TIMER0_COMP_vect)
{
	++time_count;
	if (time_count > max_timer)
	{
		
		time_count = 0;
		
		
		reg_switcher ^= 1;
		if (reg_switcher)
		PORTD &= ~(1 << 7);
		else
		PORTD |=  (1 << 7);
	}
}

int main(void)
{
	init();
	
	while (1)
	{
		if (ADCSRA & (1 << ADIF))
		{
			char t1 = ADCL;
			char t2 = ADCH;
			
			
			short int temp = t2;
			
			temp = temp >> 2;
			max_timer = temp;
		}
	}
}