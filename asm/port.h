#ifndef _PORT_H
#define _PORT_H

/*================================================*/
/************����MCU I/O�� start*******************/
//1�������0������
//PORTA  �������
#define LCD_data PORTA

//PORTB  �������
#define K1_8_PORT    PORTB
#define K1   0
#define K2   1
#define K3   2
#define K4   3
#define K5   4
#define K6   5
#define K7   6
#define K8   7

//PORTC  �������
#define K9_16_PORT    PORTC
#define K9        0
#define K10       1
#define K11       2
#define K12       3
#define K13       4
#define K14       5
#define K15       6
#define K16       7

//PORTD
#define K25_PORT    PORTD
#define BELL_PORT   PORTD
#define relay19  0  //�̵���
#define relay26  1  //�����̵���
#define rxd1     2  //RXD���룬��������Ч      ;USART1�������ڽ�PC������
#define txd1     3  //TXD���                  ����USART1�����ݿں�ʱ�ӿ�
#define key1     4  //�Զ����Կ�ʼ�İ���,����
#define key2     5  //�ֶ����Եİ���������
#define bell     6  //���������ƣ�pnp�ܣ��͵�ƽ��Ч��  ���
#define K25      7  //�̵���

//PORTE
#define K26_29_PORT    PORTE
#define rxd0     0  //RXD���룬��������Ч      ;USART0�������ڽ�FLUKE���ñ�ĸ��
#define txd0     1  //TXD���                  ����USART0�����ݿں�ʱ�ӿ�
#define LCD_RS   2
#define LCD_EN   3  //LCD�Ŀ��ƿڣ����
#define K26      4
#define K27      5
#define K28      6
#define K29      7  //4���̵������ƣ����

//PORTF
#define K17_24_PORT    PORTF
#define K24   0
#define K23   1
#define K22   2
#define K21   3
#define K20   4
#define K19   5
#define K18   6
#define K17   7

//PORTG
#define K30_32_PORT    PORTG
#define K30    0
#define K31    1
#define K32    2
#define LED2   3
#define K20    4

/************����MCU I/O�� end*********************/
/*================================================*/

/*================================================*/
/************����MCU Flag��־λ start**************/
#pragma global_register flag1:20 flag2:21 flag3:22 flagerr:23

unsigned char flag1;

#define keyprq_flag1     0	//�м����±�־λ
#define keyeff_flag1     1	//������Ч��־λ
#define Usart0_RECVFlag1 2 //=1��ʾ����0���յ���һ�����������ݰ�
#define Usart1_RECVFlag1 3 //=1��ʾ����1���յ���һ�����������ݰ�
#define error_flag1      4   //ͨ�Ŵ����־λ
#define AD0_RECVFlag1    5 //=1��ʾADC0���յ���һ�����������ݰ�
#define time_goFlag1     6
#define recv_okFlag1     7

unsigned char flag2;
#define slave_initFlag2 0
#define step_NGFlag2 1
unsigned char flag3;
unsigned char flagerr;

/************����MCU Flag��־λ end****************/
/*================================================*/

#endif