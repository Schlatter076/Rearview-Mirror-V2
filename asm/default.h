#ifndef _DEFAULT_H
#define _DEFAULT_H

#define TRUE 1
#define FALSE 0
#define uart_first_text   0xf3
#define uart_second_text  0xf4
#define uart_end_text    0xfc  //结束字节

#define BAUD 9600           //波特率采用9600b/s
#define CRYSTAL 11059200UL    //系统时钟11.0592M

//计算和定义波特率设置参数
#define BAUD_SETTING (unsigned int) ((unsigned long)CRYSTAL/(16*(unsigned long)BAUD)-1)
#define BAUD_H (unsigned char)(BAUD_SETTING>>8)
#define BAUD_L (unsigned char)(BAUD_SETTING)

//USART0
#define USART0_FRAMING_ERROR (1<<FE0)
#define USART0_PARITY_ERROR (1<<PE0)  //PE
#define USART0_DATA_OVERRUN (1<<DOR0)
#define USART0_DATA_REGISTER_EMPTY (1<<UDRE0)

//USART1
#define USART1_FRAMING_ERROR (1<<FE1)
#define USART1_PARITY_ERROR (1<<PE1)  //PE
#define USART1_DATA_OVERRUN (1<<DOR1)
#define USART1_DATA_REGISTER_EMPTY (1<<UDRE1)

#define set_bit(x,y) (x|=(1<<y)) //置1功能
#define clr_bit(x,y) (x&=~(1<<y)) //清0功能
#define xor_bit(x,y) (x^=(1<<y)) //取反功能
#define bit(x) (1<<x)            //对某位操作
#define get_bit(x,y) (x&=(1<<y)) //取出某位 


#endif