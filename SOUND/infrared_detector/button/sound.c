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
	// ��������� ������� ���������
	DDRB	   |=  (1 << 1);							// ���� - �� �����
	DDRD       |=  (1 << 7);                            //  PD7 �� �����
	PORTB      &= ~(1 << 1);							// �� ��������� �������� �� �����

	// ��������� �������
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// ����� ��� ���������� �� ������� 0 �� ���������� �������� TCNT0 �������
	
	TCCR0       = 0x0A;									// b  0 0 0 0 1 0 1 0
														//      0     1        ����� CTC (����� ��� ����������)
														//              0 1 0  CS02..00 - ������������ ��� ������� �0 (����� 1/8) 
	
	OCR0        = 0x40;									// ������������ �������� �������� ���������, ������ 33d (�� ������������ �������)
														//  16 000 000 / (8 * 33) ~= 60 606 ~= 60 kHz
	
	// ����������� ���
	ADMUX       = 0x60;			// b 0 1 1 0 0 0 0 0
	//   0 1              �������� �������� ���������� - AVcc = 5V
	//       1            ������������ ���������� �����  ( DDDDDDDD DDxxxxxx )
	//         0 0 0 0 0  ����� �������� ������ (ADC0, �������, ��� ��������)
	
	SFIOR      &= 0x1F;         // b 0 0 0 ? ? ? ? ?
	//   0 0 0            ����� ������������ ��������������

	ADCSRA      = 0xE1;         // b 1 1 1 0 0 1 1 1
	//   1                �������� ���� ���
	//     1              ��������� �������������� ("���������" ��������������)
	//       1            ������������� ����� ������ ��� � ������������ � ��������� SFIOR (������������ ��������������)
	//         0          ���� ���������� ����������
	//           0        ������ ���������� �� ���������� ���������� (����� �� �����)
	//             1 1 1  ������ ������������ (����� ������������: 1/128)
	
	sei();												// ���������� ���������� ����������
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