#include "uart.h"
#include	"ATCMD.H"

unsigned char	xdata	uart_array[UART_BUF_MAX];
bit	data	UART_BUSY=0;	//串口发送忙标志位1=忙

ST_UART	uart_buf=
{
	UART_BUF_MAX,	//缓冲区长度
	uart_array,			//缓冲区首地址
	0,						//有效数据长度
	0 						//起始数据位置
};

unsigned char	UART_BUF_Read(void)
{
	unsigned char	*addr;
	addr=uart_buf.P_buf + uart_buf.Buf_Start;
	if(uart_buf.Value_Data>0)
	{
	uart_buf.Value_Data--;
	uart_buf.Buf_Start++;
	if(uart_buf.Buf_Start >= uart_buf.Max_Date)	uart_buf.Buf_Start=0;
	}
	return	*addr;
}

void	UART_BUF_Write(unsigned char temp)
{
	unsigned char *addr;
	if(uart_buf.Value_Data<uart_buf.Max_Date)
	{
		if((uart_buf.Value_Data+uart_buf.Buf_Start)>=uart_buf.Max_Date)	addr=uart_buf.P_buf+uart_buf.Value_Data+uart_buf.Buf_Start-uart_buf.Max_Date;
		else	addr=uart_buf.P_buf+uart_buf.Value_Data+uart_buf.Buf_Start;
		*(addr)=temp;
		uart_buf.Value_Data++;
	}
}


/*----------------------------
    UART1初始化
-----------------------------*/
void InitUART(void)
{
	SCON = 0x50;
	AUXR |= 0x04;
	AUXR &= 0xFE;
	TMOD &= 0x0F;
	T2L = 0xDF;
	T2H = 0xFF;	
	AUXR |= 0x01;
	AUXR |= 0x10;
  ES = 1;
	PS=1;
  EA = 1;
}



/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4	
{
   if (RI)
    {
    RI = 0;                 //清除RI位
		UART_BUF_Write(SBUF);
			at_scan();
			TH0 = TIMER0_H;	
			TF0 = 0;
    }
	if(TI)
		{
		TI=0;
		UART_BUSY=0;
		}
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(unsigned char dat)
{
    while (UART_BUSY);               //等待前面的数据发送完成
    SBUF = dat;                 //写数据到UART数据寄存器
		UART_BUSY=1;								//忙标志置1
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(unsigned char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}
//定时器0初始化 用于5ms定时
void Timer0Init(void)
{
	AUXR &= 0x7F;		
	TMOD &= 0xF0;	
	TMOD |= 0x01;
	TL0 = TIMER0_L;	
	TH0 = TIMER0_H;	
	TF0 = 0;
	TR0 = 1;
}
