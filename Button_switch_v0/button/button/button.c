/* ATmega8535-16U */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

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

#define BUTTONS_POLL_INTERVAL 10

int main(void)
{
	LED_DDR    |= (1 << LED);     // ��� ���������� ����������� ��� �����
	LED_PORT   &= ~(1 << LED);    // �� ������� ���������
	BUTTON_DDR &= ~(1 << BUTTON); // ��� ������ ������������� ������ � 0
	
	char led_state = 0;
	char last_button_state = 0;
	char cur;

	while (1)
	{
		cur = (BUTTON_PIN & (1 << BUTTON));
		if (cur && !last_button_state) // ���� ������ ������
		{
			led_state = 1 - led_state;
		}
		
		if (led_state)	
			LED_PORT |= (1 << LED);		// �������� ��������� (�������� �� ����������)
		else
			LED_PORT &= ~(1 << LED);	// ����� ���������
		
		last_button_state = cur;
		_delay_ms(BUTTONS_POLL_INTERVAL);
	}
}