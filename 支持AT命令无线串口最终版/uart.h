#ifndef __uart_H_
#define __uart_H_

#include	<STC/STC15F2K60S2.H>

#define UART_BUF_MAX	250
#define	TIMER0_H	0xCF
#define	TIMER0_L	0x2C

extern	bit	data	UART_BUSY;
extern	unsigned char	xdata	uart_array[UART_BUF_MAX];

typedef	struct
{
	unsigned	char	Max_Date;	//缓冲区长度
	unsigned	char	*P_buf;		//缓冲区首地址
	unsigned	char	Value_Data;//有效数据长度
	unsigned	char	Buf_Start;	//起始数据位置
}ST_UART;
extern	ST_UART	uart_buf;
extern	unsigned char	UART_BUF_Read(void);
extern	void	UART_BUF_Write(unsigned char temp);

extern	void InitUART(void);
extern	void SendData(unsigned char dat);
extern	void SendString(unsigned char *s);
extern	void Timer0Init(void);

#endif