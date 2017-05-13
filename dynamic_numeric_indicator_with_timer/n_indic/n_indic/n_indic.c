/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // = 1

char reg_switcher = 0;
const char max_timer_count = 61;
char timer_count = 0;
char recieved_data = 0x74;

char LED_NUMBER[16] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90,
					   0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};
					   
void display_number(int x)
{
	PORTC = LED_NUMBER[x];
}

void init()
{
	// ������� ����������
	DDRC		 =  0xFF;								// ���� ���� C - �� ����� (����������)
	DDRA		|= (1 << PA6) | (1 << PA7);				// ����� ����������� (���� �� ���������� 1554��3, ����� ��� ������������ ���������)
														// PA6 - ������� ������, PA7 - �������
														
	PORTC		 =  0x00;								// ���������� ������ �� �����
	PORTA		|= (1 << PA6) | (1 << PA7);				//  PA6 � PA7 := 1
	
	// ������������� �������
	
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// ����� ��� ���������� �� ������� 0 �� ���������� OCR0 � �������� TCNT0 �������
	
	TCCR0       = 0x0D;                                 // 0b 0 0 0 0 1 1 0 1   (������������ 1/1024)
	OCR0        = 0x80;									// ������������ �������� �������� ��������� (�� ������������ �������)
	
	// ������������� COM-�����
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
	
	// ��������� ����������
	sei();												// SREG		= (1 << 7);
}

ISR (TIMER0_COMP_vect)
{
	++timer_count;
	if (timer_count > max_timer_count)
	{
		timer_count = 0;
		++recieved_data;
	}
	reg_switcher ^= 1;
	
	if (reg_switcher)								// ������� ����������
	{
		PORTA			&= ~(1 << PA6);					// PA6 := 0
		PORTA			|=  (1 << PA7);
		display_number(recieved_data & ((1 << 4) - 1));
	}
	else											// ������� ����������
	{
		PORTA			&= ~(1 << PA7);					// PA7 := 0
		PORTA			|=  (1 << PA6);
		display_number(recieved_data >> 4);
	}	
}

ISR (USART_RX_vect)
{
	recieved_data = UDR;
	
	while (!(UCSRA & (1 << UDRE))) { };

	UDR				= recieved_data;
}

int main(void)
{
	init();
	
	while (1)
	{
		
	}
}