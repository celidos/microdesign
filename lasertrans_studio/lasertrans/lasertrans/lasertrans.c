#define F_CPU 16000000

#define LED 1				// PB1 - Индикаторная лампочка		(выход)
#define BUTTON 2			// PB2 - Кнопочка					(вход)

#define BUTTONS_POLL_INTERVAL 10

#define MAXDATA 16

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char data[16];
char ndex = 0;

void init()
{
	// настройка внешних устройств
	DDRB	   |=  (1 << LED);							// Лампочка - на выход (кнопка на вход по умолчанию)
	DDRB       &= ~(1 << BUTTON);
	PORTB      |= (1 << LED);          //&= ~(1 << LED);		// По умолчанию лампочка горит
	
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
	
	ndex = 0;
	
	sei();
}

ISR (USART_RX_vect)
{
	//if (UCSRA & (1 << RXC))
	//{
		data[ndex] = UDR;
		ndex = (ndex + 1) & (MAXDATA - 1);
	//}
}

void trasmit_data()
{
	for (int i = 0; i < MAXDATA; ++i)
	{
		while (! (UCSRA & (1 << UDRE)));
		
		UDR = data[i];
	}
}

int main(void)
{
	init();
		/*DDRB	   |=  (1 << LED);							// Лампочка - на выход (кнопка на вход по умолчанию)
		DDRB       &= ~(1 << BUTTON);
		PORTB      |= (1 << LED);          //&= ~(1 << LED);		// По умолчанию лампочка горит*/
	
	
	char last_button_state = 0;
	char cur;
	char led_state = 0;

	while (1)
	{
		cur = (PINB & (1 << BUTTON));
		if (cur && !last_button_state) // Если кнопка нажата
		{
			led_state = 1 - led_state;
			trasmit_data();
			_delay_ms(BUTTONS_POLL_INTERVAL);
		}
		
		if (led_state)
		{
			PORTB |= (1 << LED);		// включаем светодиод (инверсия на микросхеме)
			
		}
		else
			PORTB &= ~(1 << LED);	// Иначе выключаем
		
		last_button_state = cur;
		
	}
}