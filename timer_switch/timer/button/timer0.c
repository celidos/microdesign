/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// PB1 - �������� (�����)
#define LED_PORT PORTB		// ������� ������
#define LED_DDR DDRB		// ����������� ������ (������� ����������� ������ ��� �� ���� ��� �� �����)
#define LED_PIN PINB		// ?
#define LED 1				// ����� ����

// PB2 - �������� (����)
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
	LED_DDR    |= (1 << LED);     // ��� ���������� ����������� ��� �����
	LED_PORT   &= ~(1 << LED);    // �� ������� ���������
	BUTTON_DDR &= ~(1 << BUTTON); // ��� ������ ������������� ������ � 0
	
	TIMSK       = (1 << BIT_TIMER0_COMPARE_INTERRUPT);	// ����� ��� ���������� �� ������� 0 �� ���������� OCR0 � �������� TCNT0 �������
	
	TCCR0       = 0x0D;//(1 << WGM01) | (1 << CS02) | (1 << CS00); // 0b 0 0 0 0 1 1 0 1   (������������ 1/1024)
	OCR0        = 0xFF;									// ������������ �������� �������� ��������� (�� ������������ �������)
	
	sei();												// SREG		= (1 << 7);
	
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
		if (BUTTON_PIN & (1 << BUTTON))
			max_timer_count = 61;			// �������� ���������, ����� �����
		else
			max_timer_count = 6;		// ����� ���������
		/*cur = (BUTTON_PIN & (1 << BUTTON));
		if (cur && !last_button_state)			// ���� ������ ���� ������
		{
			flashing_enabled ^= 1;				//  ����������� ���������
			
		}
		_delay_ms(BUTTONS_POLL_INTERVAL);*/
	}
}