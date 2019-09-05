#ifndef _PORT_H
#define _PORT_H

/*================================================*/
/************定义MCU I/O口 start*******************/
//1是输出，0是输入
//PORTA  都是输出
#define LCD_data PORTA

//PORTB  都是输出
#define K1_8_PORT    PORTB
#define K1   0
#define K2   1
#define K3   2
#define K4   3
#define K5   4
#define K6   5
#define K7   6
#define K8   7

//PORTC  都是输出
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
#define relay19  0  //继电器
#define relay26  1  //两个继电器
#define rxd1     2  //RXD输入，且上拉有效      ;USART1串口用于接PC，公座
#define txd1     3  //TXD输出                  串口USART1的数据口和时钟口
#define key1     4  //自动测试开始的按键,输入
#define key2     5  //手动测试的按键，输入
#define bell     6  //蜂鸣器控制，pnp管，低电平有效。  输出
#define K25      7  //继电器

//PORTE
#define K26_29_PORT    PORTE
#define rxd0     0  //RXD输入，且上拉有效      ;USART0串口用于接FLUKE万用表，母座
#define txd0     1  //TXD输出                  串口USART0的数据口和时钟口
#define LCD_RS   2
#define LCD_EN   3  //LCD的控制口，输出
#define K26      4
#define K27      5
#define K28      6
#define K29      7  //4个继电器控制，输出

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

/************定义MCU I/O口 end*********************/
/*================================================*/

/*================================================*/
/************定义MCU Flag标志位 start**************/
#pragma global_register flag1:20 flag2:21 flag3:22 flagerr:23

unsigned char flag1;

#define keyprq_flag1     0	//有键按下标志位
#define keyeff_flag1     1	//按键有效标志位
#define Usart0_RECVFlag1 2 //=1表示串口0接收到了一个完整的数据包
#define Usart1_RECVFlag1 3 //=1表示串口1接收到了一个完整的数据包
#define error_flag1      4   //通信错误标志位
#define AD0_RECVFlag1    5 //=1表示ADC0接收到了一个完整的数据包
#define time_goFlag1     6
#define recv_okFlag1     7

unsigned char flag2;
#define slave_initFlag2 0
#define step_NGFlag2 1
unsigned char flag3;
unsigned char flagerr;

/************定义MCU Flag标志位 end****************/
/*================================================*/

#endif