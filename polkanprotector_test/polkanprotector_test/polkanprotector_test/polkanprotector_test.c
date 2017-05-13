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

	while (1)
	{
		if (!(BUTTON_PIN & (1 << BUTTON))) // ���� ������ ������
		{
			LED_PORT |= (1 << LED);		// �������� ��������� (�������� �� ����������)
		}
		else
		{
			LED_PORT &= ~(1 << LED);	// ����� ���������
		}
		_delay_ms(BUTTONS_POLL_INTERVAL);
	}
}
