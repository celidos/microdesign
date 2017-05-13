/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED 1				// PB1 - Индикаторная лампочка		(выход)	
#define BUTTON 2			// PB2 - Кнопочка					(вход)		

char recieved_data = 0;
char flagRX = 0;

void init()
{
	// настройка внешних устройств
	DDRB	   |=  (1 << LED);							// Индикатор и мигающий диод - на выход
	PORTB       =  0x00;          //&= ~(1 << LED);		// По умолчанию лампочка не горит
	
	UCSRA       =  0x00;	// b 0 0 0 0 0 0 0 0 
							//   0                Флаг - прием завершен (если стоит 1)
							//     0              Флаг - передача заврешена (если стоит 1)
							//       0            0 показывает, что буфер пустой
							//         0 0 0      Ошибки не обрабатываем
							//               0    Двойная скорость не нужна
							//                 0  Наркоманский режим,который не нужен
							
	UCSRB		=  0x98;	// 
	UCSRC		=  0x86;    //
	UBRRH		=  0x00;    // Старший байт скорости USART - скорость 9600 влезает в один байт
	UBRRL		=  0x67;    // 103d - задает скорость 9600 бит/с
	
	sei();
}

ISR (USART_RX_vect)
{
	//if (UCSRA & (1 << RXC))
	//{
		recieved_data = UDR;
		PORTB = recieved_data;
		//flagRX = 0x01;
	//}
	
	while (!(UCSRA & (1 << UDRE))) { };

	UDR				= recieved_data;
	recieved_data	= 0;
	//flagRX			= 0;
	
}

char f;

int main(void)
{
	init();
	
	while (1)
	{
		if (1)
			f = f;
		
		/*if (flagRX == 0x01)
		{
			
		}*/
	}
}