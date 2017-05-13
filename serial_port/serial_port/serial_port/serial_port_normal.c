/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED 1				// PB1 - ������������ ��������		(�����)	
#define BUTTON 2			// PB2 - ��������					(����)		

char recieved_data = 0;
char flagRX = 0;

void init()
{
	// ��������� ������� ���������
	DDRB	   |=  (1 << LED);							// ��������� � �������� ���� - �� �����
	PORTB       =  0x00;          //&= ~(1 << LED);		// �� ��������� �������� �� �����
	
	UCSRA       =  0x00;	// b 0 0 0 0 0 0 0 0 
							//   0                ���� - ����� �������� (���� ����� 1)
							//     0              ���� - �������� ��������� (���� ����� 1)
							//       0            0 ����������, ��� ����� ������
							//         0 0 0      ������ �� ������������
							//               0    ������� �������� �� �����
							//                 0  ������������ �����,������� �� �����
							
	UCSRB		=  0x98;	// 
	UCSRC		=  0x86;    //
	UBRRH		=  0x00;    // ������� ���� �������� USART - �������� 9600 ������� � ���� ����
	UBRRL		=  0x67;    // 103d - ������ �������� 9600 ���/�
	
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