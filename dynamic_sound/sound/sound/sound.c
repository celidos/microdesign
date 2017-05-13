/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED 1				// PB1 - ������������ ��������		(�����)	
#define BUTTON 2			// PB2 - ��������					(����)		
#define IRED 3				// PB3 - ������������ ���������		(�����)		
#define TSOP 4				// PB4 - TSOP - ������������ ������ (����)

#define BIT_TIMER0_COMPARE_INTERRUPT OCIE0 // -> 1
//#define BIT_TIMER0_OVERFLOW_INTERRUPT TOIE0 // -> 1

#define BUTTONS_POLL_INTERVAL 20

void init()
{
	// ��������� ������� ���������
	DDRB	   |=  (1 << LED) | (1 << IRED);			// ��������� � �������� ���� - �� �����
	DDRB       &= ~((1 << BUTTON) | (1 << TSOP));		// ������ � TSOP - �� ����
	PORTB      &= ~((1 << LED) | (1 << IRED));			// �� ��������� �������� �� �����

	// ��������� �������
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// ����� ��� ���������� �� ������� 0 �� ���������� �������� TCNT0 �������
	
	TCCR0       = 0x0A;									// b  0 0 0 0 1 0 1 0
														//      0     1        ����� CTC (����� ��� ����������)
														//              0 1 0  CS02..00 - ������������ ��� ������� �0 (����� 1/8) 
	
	OCR0        = 0x21;									// ������������ �������� �������� ���������, ������ 33d (�� ������������ �������)
														//  16 000 000 / (8 * 33) ~= 60 606 ~= 60 kHz
	 
	sei();												// ���������� ���������� ����������
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
		if (!(PINB & (1 << TSOP))) // ���� ��� �����������, �� �� ������ TSOP ������ ����
			PORTB |= (1 << LED);
		else
			PORTB &= ~(1 << LED);
	}
}