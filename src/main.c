#include <iom64v.h>
#include <stdio.h>
#include <macros.h>
#include <port.h>
#include <default.h>
#include <delay.h>
#include <EEPROM.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


/********�������� start*********************/
#define false 0
#define true  1
#define KEY_COUNTER 5
struct
{
    unsigned char now;
    unsigned char code;
    unsigned char old;
    unsigned char cnt;
} KEY;
//���Գ����ԼӼ�����
unsigned char auto_counter = 0;
//ָ���
static unsigned char steps[] = {0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0a};
unsigned char reset[] = {0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0a};
unsigned char start[] = {0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0a};
#define Insulation_PASS    ((PING&(1<<PG4)) == 0)
#define Insulation_FAIL    ((PING&(1<<PG3)) == 0)
static volatile unsigned char Insulation_status = 0;

//��������=======================================================================
void init_cpu(void);
void key_scan(void);
void key_process(void);
void USART0_putchar(unsigned char c);
void UART0_send(unsigned char buff[]);
void UART0_send_multiple(unsigned char buff[], unsigned char times, unsigned int delays);
void init_usart0(void);
void step_ctr(unsigned char step, unsigned char mode, unsigned char val);
void rearview_mirror(void);
void open_Insulation(void);
void reset_Insulation(void);
unsigned char Insulation_step(unsigned char step);
void volDrop_step(unsigned char step);
void NG(void);
//��������=======================================================================
//===============================���ڲ�������Start===================================
//����0���ջ�������С
#define RX_BUFFER_SIZE 11
//����0���ͻ�������С
#define TX_BUFFER_SIZE 11
//����0�����ṹ��
struct
{
    unsigned char rx_buffer[RX_BUFFER_SIZE]; //���ջ�����
    unsigned char rx_counter; //���ջ������Ѷ�����
    unsigned char tx_buffer[TX_BUFFER_SIZE]; //���ͻ�����
    unsigned char tx_wr_index;  //���ͻ�����д�����
    unsigned char tx_rd_index; //���ͻ�������������
    unsigned char tx_counter; //���ͻ��������м���
} UART0;
//===============================���ڲ�������End=====================================
void init_UART0_params(void)
{
    unsigned char p_cnt;
    for(p_cnt = 0; p_cnt < RX_BUFFER_SIZE; p_cnt++)
    {
        UART0.rx_buffer[p_cnt] = 0;
    }
    for(p_cnt = 0; p_cnt < TX_BUFFER_SIZE; p_cnt++)
    {
        UART0.tx_buffer[p_cnt] = 0;
    }
    UART0.rx_counter = 0;
    UART0.tx_wr_index = 0;
    UART0.tx_rd_index = 0;
    UART0.tx_counter = 0;
}
/***************USART0��ʼ������ start*************************/
void init_usart0(void)
{
    UCSR0B = 0x00;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  //�첽��8λ���ݣ�����żУ�飬һ��ֹͣλ���ޱ���
    UBRR0L = BAUD_L;                        //�趨������
    UBRR0H = BAUD_H;
    UCSR0A = 0x00;
    UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0); //0XD8  ���ա�����ʹ�ܣ� ���ж�ʹ��
}
/***************USART0��ʼ������ end***************************/
/***********USART0�����жϷ����� start**********************/
//����һ����־λUsart0_RECVFlag1:=1��ʾ����0���յ���һ�����������ݰ�
//��port.h�ж���

#pragma interrupt_handler usart0_rxc_isr:19  //�����жϷ������
void usart0_rxc_isr(void)
{
    unsigned char ut_status, ut_data;
    ut_status = UCSR0A;
    ut_data = UDR0;
    if((flag1 & (1 << Usart0_RECVFlag1)) == 0) //�ж��Ƿ��������һ���µ����ݰ�
    {
        if ((ut_status & (USART0_FRAMING_ERROR | USART0_PARITY_ERROR | USART0_DATA_OVERRUN)) == 0)
        {
            UART0.rx_buffer[UART0.rx_counter] = ut_data;
            UART0.rx_counter++;
            switch (UART0.rx_counter)
            {
            case 1:       // ������ʼ�ַ�
            {
                if (ut_data != 0xf3) UART0.rx_counter = 0;

            }
            break;
            case 2:
            {
                if (ut_data != 0xf4) UART0.rx_counter = 0;

            }
            break;
            case 11:      // ��������ַ�
            {
                UART0.rx_counter = 0;
                if(ut_data == 0x0a)
                    set_bit(flag1, Usart0_RECVFlag1); // Usart0_RecvFlag=1����ʾ��ȷ���յ�һ�����ݰ�
            }
            break;
            default:
                break;
            }
        }
    }
}
/***************USART0�����жϷ����� end**********************/
/*============================================================*/
/*============================================================*/
/***************USART0�����жϷ����� start********************/
#pragma interrupt_handler usart0_txc_isr:21  //�����жϷ������
void usart0_txc_isr(void)
{
    if (UART0.tx_counter)//���в�Ϊ��
    {
        --UART0.tx_counter;//������
        UDR0 = UART0.tx_buffer[UART0.tx_rd_index];
        if (++UART0.tx_rd_index == TX_BUFFER_SIZE) UART0.tx_rd_index = 0;
    }
}
/***********USART0�����жϷ����� end**********************/

/*============================================================*/
/***********USART0����һ���ַ����� start**********************/
void USART0_putchar(unsigned char c)
{
    while (UART0.tx_counter == TX_BUFFER_SIZE);
    CLI();//#asm("CLI")�ر�ȫ���ж�����
    if (UART0.tx_counter || ((UCSR0A & USART0_DATA_REGISTER_EMPTY) == 0)) //���ͻ�������Ϊ��
    {
        UART0.tx_buffer[UART0.tx_wr_index] = c; //���ݽ������
        if (++UART0.tx_wr_index == TX_BUFFER_SIZE) UART0.tx_wr_index = 0; //��������
        ++UART0.tx_counter;
    }
    else
        UDR0 = c;
    SEI(); //#asm("SEI")��ȫ���ж�����
}
/***********USART0���ͷ����� end**********************/
/*============================================================*/
/**
* ����0����һ��ָ��
* @buff ָ�����飬����ΪTX_BUFFER_SIZE
*/
void UART0_send(unsigned char buff[])
{
    unsigned char buff_cnt;
    for(buff_cnt = 0; buff_cnt < TX_BUFFER_SIZE; buff_cnt++)
    {
        USART0_putchar(buff[buff_cnt]);
    }
}
/**
* ����0���Ͷ��ָ��
* @buff ָ�����飬����ΪTX_BUFFER_SIZE_1
* @times ���͵Ĵ���
* @dealys ÿ�η��͵�ʱ����(��λms����)
*/
void UART0_send_multiple(unsigned char buff[], unsigned char times, unsigned int delays)
{
    unsigned char i = 0;
    do
    {
        if(i++ >= times)
        {
            i = 0;
            break; //���ͳ�ʱ������ѭ��
        }
        UART0_send(buff);
        delay_nms(delays);
    }
    while(1);
}
/*============================================================*/
/************************************************************************/
/** ������ƺ���
 * @step ������
 * @mode:0x22--��ȡ���� 0x33--��ȡ��ѹ�� 0x44--��Ե���Խ������
 * @val ѡ���Ե���Խ������ʱֵ��0x10��ʾok��0x11��ʾfail
 */
/************************************************************************/
void step_ctr(unsigned char step, unsigned char mode, unsigned char val)
{
    steps[2] = step;
    steps[3] = val;
    steps[9] = mode;
    delay_nms(10);
    //UART0_send(steps);
}

void open_Insulation(void)
{
    delay_nms(200);
    set_bit(K17_24_PORT, K21);
    delay_nms(100);
    clr_bit(K17_24_PORT, K21);
    delay_nms(200);
    while(!Insulation_PASS && !Insulation_FAIL);
}
void reset_Insulation(void)
{
    set_bit(K17_24_PORT, K22);
    delay_nms(100);
    clr_bit(K17_24_PORT, K22);
}
unsigned char Insulation_step(unsigned char step)
{
    open_Insulation();
    if(Insulation_FAIL)
    {
        delay_nms(10);
		if(Insulation_FAIL)
		{
		    step_ctr(step, 0x44, 0x11);
        	UART0_send_multiple(steps, 3, 150);
        	clr_bit(flag1, Usart0_RECVFlag1);
			//reset_Insulation();
        	return 1;
		}
    }
    step_ctr(step, 0x44, 0x10);
    UART0_send_multiple(steps, 3, 150);
    clr_bit(flag1, Usart0_RECVFlag1);
    reset_Insulation();
    delay_nms(100);
    return 0;
}
void volDrop_step(unsigned char step)
{
    //while(1);
    delay_nms(500);
    step_ctr(step, 0x33, 0);
    UART0_send_multiple(steps, 3, 100);
    //delay_nms(100);
    while((flag1 & (1 << Usart0_RECVFlag1)) == 0);
    //delay_nms(20);
    clr_bit(flag1, Usart0_RECVFlag1);
    //*/
    //delay_nms(100);
    //UART0.rx_buffer[2] = 0x10;
}
void NG(void)
{
    set_bit(K30_32_PORT, K30);
    set_bit(flag2, step_NGFlag2);
}

void rearview_mirror(void)
{
    auto_counter++;
    switch(auto_counter)
    {
    case 1:
    {
        UART0_send_multiple(start, 3, 200);
        if((flag1 & (1 << Usart0_RECVFlag1)) == 0)
        {
            auto_counter = 0;
            return;
        }
        clr_bit(flag1, Usart0_RECVFlag1);
        delay_nms(200);
        //step1 �������
        set_bit(K1_8_PORT, K1);
        set_bit(K1_8_PORT, K2);
        delay_nms(200);

        step_ctr(1, 0x22, 0);
        UART0_send(steps);
        while(!(flag1 & (1 << Usart0_RECVFlag1)));
        clr_bit(flag1, Usart0_RECVFlag1);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        clr_bit(K1_8_PORT, K1);
        clr_bit(K1_8_PORT, K2);
        //step2 7-1��Ե
        set_bit(K1_8_PORT, K3);
        set_bit(K1_8_PORT, K4);
        set_bit(K1_8_PORT, K6);
        Insulation_status = Insulation_step(2);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K1_8_PORT, K6);
        set_bit(K1_8_PORT, K7);
        Insulation_status = Insulation_step(3);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K1_8_PORT, K7);
        set_bit(K1_8_PORT, K8);
        Insulation_status = Insulation_step(4);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K1_8_PORT, K8);
        set_bit(K9_16_PORT, K9);
        Insulation_status = Insulation_step(5);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K9_16_PORT, K9);
        set_bit(K9_16_PORT, K10);
        Insulation_status = Insulation_step(6);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K9_16_PORT, K10);
        set_bit(K9_16_PORT, K11);
        Insulation_status = Insulation_step(7);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K9_16_PORT, K11);
        clr_bit(K1_8_PORT, K4);

        set_bit(K1_8_PORT, K5);
        set_bit(K1_8_PORT, K7);
        Insulation_status = Insulation_step(8);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K1_8_PORT, K7);
        set_bit(K1_8_PORT, K8);
        Insulation_status = Insulation_step(9);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K1_8_PORT, K8);
        set_bit(K9_16_PORT, K9);
        Insulation_status = Insulation_step(10);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K9_16_PORT, K9);
        set_bit(K9_16_PORT, K10);
        Insulation_status = Insulation_step(11);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K9_16_PORT, K10);
        set_bit(K9_16_PORT, K11);
        Insulation_status = Insulation_step(12);
        if(Insulation_status == 1)
        {
            NG();
            return;
        }
        clr_bit(K9_16_PORT, K11);
        clr_bit(K1_8_PORT, K5);
        delay_nms(100);
        clr_bit(BELL_PORT, bell);
        delay_nms(100);
        set_bit(BELL_PORT, bell);
        delay_nms(100);
        clr_bit(BELL_PORT, bell);
        delay_nms(100);
        set_bit(BELL_PORT, bell);

        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K25_PORT, K25);
        set_bit(K26_29_PORT, K26);
        set_bit(K26_29_PORT, K27);
        set_bit(K26_29_PORT, K28);
        set_bit(K26_29_PORT, K29);
    }
    break;
    /*
    case 2:
    {
        //L-UP
        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K14);
        set_bit(K9_16_PORT, K15);
        set_bit(K17_24_PORT, K17);
        volDrop_step(13);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K17);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K13);
        set_bit(K9_16_PORT, K16);
        volDrop_step(14);
    	delay_nms(200);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K9_16_PORT, K16);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K25_PORT, K25);
        set_bit(K26_29_PORT, K26);
    }break;
    case 3:
    {
        //L-Down
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K25_PORT, K25);
        clr_bit(K26_29_PORT, K26);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K13);
        set_bit(K17_24_PORT, K17);
        volDrop_step(15);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K17_24_PORT, K17);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K14);
        set_bit(K9_16_PORT, K16);
        volDrop_step(16);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K9_16_PORT, K16);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K25_PORT, K25);
        set_bit(K26_29_PORT, K26);
    }break;
    case 4:
    {
        //L-Left
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K25_PORT, K25);
        clr_bit(K26_29_PORT, K26);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K14);
        set_bit(K17_24_PORT, K19);
        volDrop_step(17);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K19);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K13);
        set_bit(K17_24_PORT, K17);
        volDrop_step(18);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K17_24_PORT, K17);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K26_29_PORT, K26);
        set_bit(K26_29_PORT, K28);
    }break;
    case 5:
    {
        //L-Right
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K26_29_PORT, K26);
        clr_bit(K26_29_PORT, K28);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K14);
        set_bit(K17_24_PORT, K17);
        volDrop_step(19);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K17);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K13);
        set_bit(K17_24_PORT, K19);
        volDrop_step(20);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K17_24_PORT, K19);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K26_29_PORT, K26);
        set_bit(K26_29_PORT, K28);
    }break;
    case 6:
    {
        //R-Up
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K26_29_PORT, K26);
        clr_bit(K26_29_PORT, K28);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K13);
        set_bit(K9_16_PORT, K16);
        volDrop_step(21);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K9_16_PORT, K16);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K14);
        set_bit(K17_24_PORT, K18);
        volDrop_step(22);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K18);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K25_PORT, K25);
        set_bit(K26_29_PORT, K27);
    }break;
    case 7:
    {
        //R-Down
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K25_PORT, K25);
        clr_bit(K26_29_PORT, K27);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K13);
        set_bit(K17_24_PORT, K18);
        volDrop_step(23);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K17_24_PORT, K18);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K14);
        set_bit(K9_16_PORT, K16);
        volDrop_step(24);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K16);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K25_PORT, K25);
        set_bit(K26_29_PORT, K27);
    }break;
    case 8:
    {
        //R-Left
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K25_PORT, K25);
        clr_bit(K26_29_PORT, K27);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K13);
        set_bit(K17_24_PORT, K18);
        volDrop_step(25);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K17_24_PORT, K18);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K14);
        set_bit(K17_24_PORT, K20);
        volDrop_step(26);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K20);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K26_29_PORT, K27);
        set_bit(K26_29_PORT, K29);
    }break;
    case 9:
    {
        //R-Right
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K17_24_PORT, K27);
        clr_bit(K17_24_PORT, K29);

        set_bit(K9_16_PORT, K12);
        set_bit(K9_16_PORT, K15);
        set_bit(K9_16_PORT, K13);
        set_bit(K17_24_PORT, K20);
        volDrop_step(27);
        clr_bit(K9_16_PORT, K13);
        clr_bit(K17_24_PORT, K20);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K9_16_PORT, K14);
        set_bit(K17_24_PORT, K18);
        volDrop_step(28);
        clr_bit(K9_16_PORT, K14);
        clr_bit(K17_24_PORT, K18);
        clr_bit(K9_16_PORT, K12);
        clr_bit(K9_16_PORT, K15);
        if(UART0.rx_buffer[2] == 0x11)
        {
            NG();
            return;
        }
        set_bit(K17_24_PORT, K23);
        set_bit(K17_24_PORT, K24);
        set_bit(K26_29_PORT, K27);
        set_bit(K26_29_PORT, K29);

        delay_nms(2000);
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K17_24_PORT, K27);
        clr_bit(K17_24_PORT, K29);
        clr_bit(K1_8_PORT, K3);
        delay_nms(100);
    }break;
    //*/
    default:
    {
        clr_bit(K1_8_PORT, K3);
        clr_bit(K17_24_PORT, K23);
        clr_bit(K17_24_PORT, K24);
        clr_bit(K25_PORT, K25);
        clr_bit(K26_29_PORT, K26);
        clr_bit(K26_29_PORT, K27);
        clr_bit(K26_29_PORT, K28);
        clr_bit(K26_29_PORT, K29);
        auto_counter = 0;
    }
    break;
    }
}

/***************ϵͳ��ʼ������ start ***********/
void init_cpu(void)
{
    EIMSK = 0x00; //����INT0~INT1�������ⲿ�ж�
    clr_bit(SFIOR, PUD); //������������������Ч

    DDRA = 0xff; //1�������0������
    PORTA = 0x00;

    DDRB = 0xff; //1�������0������
    PORTB = 0x00;

    DDRC = 0xff; //
    PORTC = 0x00;

    DDRD  = 0xcf; //RXD1���룬��������Ч
    PORTD = 0x40;  //TXD1�����

    DDRE = 0xfe; //RXD0���룬��������Ч
    PORTE = 0x01; //TXD0���

    DDRF = 0xff;
    PORTF = 0x00; //8���̵������

    DDRG = 0x07;
    PORTG = 0xf8;

    init_UART0_params();
    init_usart0();
    SEI();

    flag1 = 0;
    flag2 = 0;
    flag3 = 0;
    flagerr = 0;
}
/***************ϵͳ��ʼ������ end *************/
/***************������⺯�� start *************/
void key_scan(void)
{
    if((flag1 & (1 << keyprq_flag1)) == 0)
    {
        if((PIND & (1 << key1)) == 0) KEY.now = 1; //auto����
        else if((PIND & (1 << key2)) == 0) KEY.now = 2; //men����
        else
        {
            KEY.now = 0;
            KEY.code = 0;
            KEY.old = 0;
        }
        if(KEY.now != 0)
        {
            if(KEY.now != KEY.code)
            {
                KEY.code = KEY.now;
                KEY.cnt = 0;
            }
            else
            {
                KEY.cnt++;
                if(KEY.cnt >= KEY_COUNTER) //ƽ��״������10��,���ǲ���VR1ʱ����1��
                {
                    KEY.cnt = 0;
                    set_bit(flag1, keyprq_flag1);
                }
            }
        }
    }
}
/***************������⺯�� end ***************/
/*---------------------------------------------*/
/***************���������� start *************/
void key_process(void)
{
    if((flag1 & (1 << keyprq_flag1)) != 0)
    {
        clr_bit(flag1, keyprq_flag1);
        if(KEY.code == KEY.old)
        {
            ;
        }
        else
        {
            KEY.old = KEY.code;
            set_bit(flag1, keyeff_flag1);
        }
        if((flag1 & (1 << keyeff_flag1)) != 0)
        {
            clr_bit(flag1, keyeff_flag1);
            switch(KEY.old)
            {
            case 1:  //auto����
            {
                if(!(flag2 & (1 << step_NGFlag2)))
                {
                    rearview_mirror();
                }
            }
            break;

            case 2:  //men����
            {
                /*
                set_bit(K1_8_PORT, K1);
                set_bit(K1_8_PORT, K2);
                set_bit(K1_8_PORT, K3);
                set_bit(K1_8_PORT, K4);
                set_bit(K1_8_PORT, K5);
                set_bit(K1_8_PORT, K6);
                set_bit(K1_8_PORT, K7);
                set_bit(K1_8_PORT, K8);
                //*/

            } break; //*/
            default:
                break;
            }
        }
    }
}

void main (void)
{
    init_cpu();
    delay_nms(200);
    UART0_send(reset);
    delay_nms(100);
    while (1)
    {
        key_scan();
        key_process();
        delay_nms(10);
    }
}