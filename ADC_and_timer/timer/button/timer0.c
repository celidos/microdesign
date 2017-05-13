/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// PA0 (ADC0 - ������� ����� ���) - ������ �������������, ������� ���� ������
#define RESIST_PORT PORTA	// ������� ������ ����� �
#define RESIST_DDR DDRA		// ������ ����������� ����� � (�� ���� ��� �� �����)
#define RESIST_PIN PINA		// ����
#define RESIST 0			// ������������ �� ������� �����

// PB1 - �������� (�����)
#define LED_PORT PORTB		// ������� ������
#define LED_DDR DDRB		// ����������� ������ (������� ����������� ������ ��� �� ���� ��� �� �����)
#define LED_PIN PINB		// ����
#define LED 1				// ����� ����

// PB2 - �������� (����)
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
	// ��������� ������� ���������
	RESIST_DDR  = 0;              // ��� ���� �����, �� ������� ����� ������������, ����������� ��� �����
	
	LED_DDR    |= (1 << LED);     // ��� ���������� ����������� ��� �����
	LED_PORT   &= ~(1 << LED);    // �� ������� ��������� ��������� (� ������� ������ ������ 0)
	BUTTON_DDR &= ~(1 << BUTTON); // ��� ������ ������������� ������ � 0 (�.�. ������ ��� ����)
	
	// �������� �������
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// ����� ��� ���������� �� ������� 0 �� ���������� OCR0 � �������� TCNT0 �������
	
	TCCR0       = 0x0D;//(1 << WGM01) | (1 << CS02) | (1 << CS00); // 0b 0 0 0 0 1 1 0 1   (������������ 1/1024)
	OCR0        = 0xFF;									// ������������ �������� �������� ��������� (�� ������������ �������)
	
	sei();												// SREG		= (1 << 7);
	
	// ����������� ���
	ADMUX       = 0x60;			// b 0 1 1 0 0 0 0 0
	                            //   0 1              �������� �������� ���������� - AVcc = 5V
								//       1            ������������ ���������� �����  ( DDDDDDDD DDxxxxxx )
								//         0 0 0 0 0  ����� ��������� ������ (ADC0, �������, ��� ��������)
	
	SFIOR      &= 0x1F;         // b 0 0 0 ? ? ? ? ? 
								//   0 0 0            ����� ������������ ��������������

	ADCSRA      = 0xE7;         // b 1 1 1 0 0 1 1 1 
								//   1                �������� ���� ���
								//     1              ��������� �������������� ("���������" ��������������)
								//       1            ������������� ����� ������ ��� � ������������ � ��������� SFIOR (������������ ��������������)
								//         0          ���� ���������� ����������
								//           0        ������ ���������� �� ���������� ���������� (����� �� �����)
								//             1 1 1  ������ ������������ (����� ������������: 1/128)
	
	timer_count	= 0;									// �������� �������
	led_state   = 0;									// ���������� ���������
}

ISR (TIMER0_COMP_vect)
{
	++timer_count;
	if (timer_count >= max_timer_count)
	{
		led_state ^= 1;
		timer_count = 0;
		
		if (led_state)
			LED_PORT |= (1 << LED);			// �������� ���������, ����� �����
		else
			LED_PORT &= ~(1 << LED);		// ����� ���������
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
		if (cur && !last_button_state)			// ���� ������ ���� ������
		{
			flashing_enabled ^= 1;				//  ����������� ���������
			
		}
		_delay_ms(BUTTONS_POLL_INTERVAL);*/
	}
}