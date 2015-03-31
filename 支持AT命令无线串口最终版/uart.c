#include "uart.h"
#include	"ATCMD.H"

unsigned char	xdata	uart_array[UART_BUF_MAX];
bit	data	UART_BUSY=0;	//���ڷ���æ��־λ1=æ

ST_UART	uart_buf=
{
	UART_BUF_MAX,	//����������
	uart_array,			//�������׵�ַ
	0,						//��Ч���ݳ���
	0 						//��ʼ����λ��
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
    UART1��ʼ��
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
UART �жϷ������
-----------------------------*/
void Uart() interrupt 4	
{
   if (RI)
    {
    RI = 0;                 //���RIλ
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
���ʹ�������
----------------------------*/
void SendData(unsigned char dat)
{
    while (UART_BUSY);               //�ȴ�ǰ������ݷ������
    SBUF = dat;                 //д���ݵ�UART���ݼĴ���
		UART_BUSY=1;								//æ��־��1
}

/*----------------------------
�����ַ���
----------------------------*/
void SendString(unsigned char *s)
{
    while (*s)                  //����ַ���������־
    {
        SendData(*s++);         //���͵�ǰ�ַ�
    }
}
//��ʱ��0��ʼ�� ����5ms��ʱ
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
