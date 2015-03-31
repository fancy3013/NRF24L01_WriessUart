/****************************************
NRF24L01库函数
STC15F2K61S2已测试OK
支持24L01+和BK2423
支持ACK和NOACK
支持发送完自动切换到接收模式
中断处理消息
制作人:Fancy 
QQ:652774317
******************************************/

#include	<STC/STC15F2K60S2.H>
#include	"NRF24L01.H"
#include "uart.h"

unsigned char RF_CH=0x40;		//RF频道
unsigned char NRF24L01_FLAG=0;	//NRF24L01当前状态标志
																//DB7; 1接收模式 			0发射模式
																//DB6; 1有任务   			0空闲
																//DB5; 1发送成功 			0发送失败(配合DB6检测是否发送成功)
																//DB4; 1NRF正常连接 	0 检测不到连接
																//DB3; 1接收到数据包 	0 缓冲区无数据
																//DB2; 1 ACK模式      0 NOACK模式
																//DB1; 1发送完自动进入接收模式 0关闭自动切换模式
unsigned char DYNPD_LEN=0;			//用于存放动态数据包长度
unsigned char TX_ADDRESS[NRF24L01_ADR_LEN]= {0x12,0x34,0x56,0x78,0x90};	//本地地址
unsigned char RX_ADDRESS[NRF24L01_ADR_LEN]= {0x12,0x34,0x56,0x78,0x90};	//接收地址


//向从机发送一个字节数据并返回接收数据
/**************************************************/

unsigned	char SPI_WriteRead(unsigned char byte)
{
	
	SPSTAT |= 0Xc0; // 清高两位，
	SPDAT=byte;
	while(!(SPSTAT&0X80));
	return SPDAT;
}

/**********************/
/* 初始化硬件SPI口    */
/**********************/
void SPI_Init(void)
{
	SPSTAT |= 0XC0; 
	SPCTL = 0XD0; 
}
//写NRF24L01寄存器，返回状态值
unsigned char NRF24L01_Write_Reg(unsigned char reg, unsigned char value)
{
	 unsigned char BackDate;
   SPI_SS=0;
   SPI_WriteRead(reg);
   BackDate=SPI_WriteRead(value);
   SPI_SS=1;
   return(BackDate);
}
//读NRF24L01寄存器，返回寄存器值
unsigned char NRF24L01_Read_Reg(unsigned char reg)
{
	 unsigned char BackDate;
   SPI_SS=0;
   SPI_WriteRead(reg);
   BackDate=SPI_WriteRead(0x00);
   SPI_SS=1;
   return(BackDate);
}
//读取缓冲区数据
void NRF24L01_Read_Buf(unsigned char reg, unsigned char *pBuf,unsigned char bytes)
{
	unsigned char byte_ctr;
	SPI_SS = 0;
	SPI_WriteRead(reg);
	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
	pBuf[byte_ctr] = SPI_WriteRead(0x00);
	SPI_SS = 1;
}
//向缓冲区写入数据
unsigned char NRF24L01_Write_Buf(unsigned char reg,unsigned char *pBuf,unsigned char bytes)
{
	unsigned char status,byte_ctr;
	SPI_SS = 0;
	status = SPI_WriteRead(reg);
	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) 
	SPI_WriteRead(*pBuf++);
	SPI_SS = 1; 
	return(status);
}

//NRF24L01初始化函数
void NRF24L01_Init(void)
{
	#if BK2423
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(ACTIVATE_CMD, 0x73);
	NRF24L01_CE=1;
	#endif
	
	NRF24L01_CE=0;
  //配置寄存器
	NRF24L01_FLAG |= NRF24L01_AUTO_ACK;		//是否开启应答模式
	NRF24L01_FLAG |= NRF24L01_AUTO_RX;		//发送完是否自动进入接收模式
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_SETUP_AW, NRF24L01_ADR_LEN-2);	//设置地址长度为 NRF24L01_ADR_LEN
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH,RF_CH);                  //设置RF通道为RFCH
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_SETUP, 0x27);               //设置TX发射参数,7db增益,250kbps,低噪声增益开启
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_EN_AA, 0x01);         					//使能通道0的自动应答
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_EN_RXADDR, 0x01);              //使能通道0的接收地址
  //RX模式配置
  NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_RX_ADDR_P0,RX_ADDRESS,NRF24L01_ADR_LEN); //写RX节点地址
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FEATURE,0x05);									//使能动态数据包长度,AUTO_ACK模式
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_DYNPD,0X01);        						//选择通道0动态数据包长度
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX,NRF24L01_NOP);                //清除RX FIFO寄存器
  //TX模式配置
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG | NRF24L01_TX_ADDR ,TX_ADDRESS,NRF24L01_ADR_LEN);	//写TX节点地址
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_SETUP_RETR, 0x1f);           //设置自动重发间隔时间:500us + 86us;最大自动重发次数:15次
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX, NRF24L01_NOP);              //清除TX FIFO寄存器
  NRF24L01_RxMode();                                                  					//默认进入接收模式
  NRF24L01_CE=1;
	IT0=1;		//下降沿中断
	EX0=1;		//开启外部中断0
	EA=1;
}

//NRF24L01进入发送模式
void NRF24L01_TxMode(void)
{
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, 0x0e);
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX,NRF24L01_NOP);  //清除TX FIFO寄存器
	NRF24L01_CE = 1; 
	NRF24L01_FLAG &= ~NRF24L01_MODE_RX;		//修改状态标识为发射模式
	NRF24L01_FLAG &= ~NRF24L01_BUSY;	//BUSY标志置0
	NRF24L01_FLAG &= ~NRF24L01_RECIVE;//清除有数据标志
}
//NRF24L01进入接收模式
void NRF24L01_RxMode(void)
{
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_CONFIG, 0x0f); 	//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_RX,NRF24L01_NOP);  //清除RX FIFO寄存器
	NRF24L01_CE = 1; 
	NRF24L01_FLAG |= NRF24L01_MODE_RX;		//修改状态标识为接收模式
	NRF24L01_FLAG &= ~NRF24L01_BUSY;	//BUSY标志置0
	DYNPD_LEN=0;	//清0动态数据包长度
	NRF24L01_FLAG &= ~NRF24L01_RECIVE;//清除有数据标志
}

//NRF24L01 SPI通信检查
void NRF24L01_Check(void)
{
	unsigned char i=0;
	unsigned char Buff[5];
	NRF24L01_Read_Buf(NRF24L01_TX_ADDR,Buff,NRF24L01_ADR_LEN);
	i=NRF24L01_Read_Reg(NRF24L01_DYNPD);	//读取DYNPD监测是否正确写入
	if(i==0x01)		//这里设置的是0x01,要与初始化的值相对应
	{
  for(i = 0; i < NRF24L01_ADR_LEN; i++)
    {
			
        if(Buff[i] == TX_ADDRESS[i])
        {
         NRF24L01_FLAG |= NRF24L01_CHECK;
        }
				else 
				{
					NRF24L01_FLAG &= ~NRF24L01_CHECK;
					break;
				}
    }
	}
	else	NRF24L01_FLAG &= ~NRF24L01_CHECK;
}

////NRF24L01发送数据
//void NRF24L01_SendFrame(unsigned char *temp,unsigned char len)
//{
//	if(NRF24L01_FLAG & NRF24L01_MODE_RX)
//	{
//		NRF24L01_TxMode();
//	}
//	NRF24L01_FLAG |= NRF24L01_BUSY;	//BUSY标志置1
//	NRF24L01_FLAG &= ~NRF24L01_TX_ACCESS;//清除发送成功标志
//	NRF24L01_CE = 0;
//	if(NRF24L01_FLAG & NRF24L01_AUTO_ACK)
//		{
//			NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD,temp,len);	//写待发数据包,需要回应
//		}
//	else
//	{
//		NRF24L01_Write_Buf(W_TX_PAYLOAD_NOACK_CMD,temp,len);	//写待发数据包,无须回应
//	}
//	NRF24L01_CE = 1; 
//}

////NRF24L01接收数据
//void NRF24L01_RecvFrame(unsigned char *temp)
//{
//	DYNPD_LEN=NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
//	if(DYNPD_LEN>32)
//	{
//		NRF24L01_RxMode();	//重新初始化接收模式
//	}
//	else
//	{
//		NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD,temp,DYNPD_LEN);	//读取缓冲区数据
//	}
//	NRF24L01_FLAG &= ~NRF24L01_RECIVE;//清除有数据标志
//}
//更改RF_CH
void NRF24L01_Change_Ch(unsigned char ch)
{
	NRF24L01_CE = 0;
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_RF_CH,ch&0x7F);  //设置RF频道
	RF_CH=ch&0x7F;					//同步设置到系统参数
}

//NRF24L01中断处理
void NRF24L01_Handler(void) 	interrupt 0	
{
	unsigned char state;
	state=NRF24L01_Read_Reg(NRF24L01_STATUS);		//读NRF24L01状态寄存器
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_STATUS,state);//清除中断
	//发送成功中断
	if(state & NRF24L01_STATUS_TX_DS)
	{
		BLUE=0;
		NRF24L01_FLAG &= ~NRF24L01_BUSY;	//BUSY标志置0
		NRF24L01_FLAG |= NRF24L01_TX_ACCESS;//发送成功标志置1
		if(NRF24L01_FLAG & NRF24L01_AUTO_RX)
		{
			NRF24L01_RxMode();	//重新初始化接收模式
		}
		BLUE=1;
	}
	//达到最大重发次数中断
	if(state & NRF24L01_STATUS_MAX_RT)
	{
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG | NRF24L01_FLUSE_TX,NRF24L01_NOP);  //清除TX FIFO寄存器
		NRF24L01_FLAG &= ~NRF24L01_BUSY;	//BUSY标志置0
		NRF24L01_FLAG &= ~NRF24L01_TX_ACCESS;//发送成功标志置0
		if(NRF24L01_FLAG & NRF24L01_AUTO_RX)
		{
			NRF24L01_RxMode();	//重新初始化接收模式
		}
	}
	//接收到数据中断
	if(state & NRF24L01_STATUS_RX_DR)
	{
		RED=0;
		NRF24L01_FLAG |= NRF24L01_RECIVE;	//接收到数据标志位置1
		NRF24L01_Recvfifo();
		RED=1;
	}
}

//环形FIFO缓冲区配置
unsigned char	idata	nrf_rxarray[NRF_RXFIFO_MAX];

ST_NRF	nrf_rxfifo=
{
	NRF_RXFIFO_MAX,	//缓冲区长度
	nrf_rxarray,		//缓冲区首地址
	0,						//有效数据长度
	0 						//起始数据位置
};

unsigned char	NRF_RXFIFO_Read(void)
{
	unsigned char	*addr;
	addr=nrf_rxfifo.P_buf + nrf_rxfifo.Buf_Start;
	if(nrf_rxfifo.Value_Data>0)
	{
	nrf_rxfifo.Value_Data--;
	nrf_rxfifo.Buf_Start++;
	if(nrf_rxfifo.Buf_Start >= nrf_rxfifo.Max_Date)	nrf_rxfifo.Buf_Start=0;
	}
	return	*addr;
}

void	NRF_RXFIFO_Write(unsigned char temp)
{
	unsigned char *addr;
	if(nrf_rxfifo.Value_Data<nrf_rxfifo.Max_Date)
	{
		if((nrf_rxfifo.Value_Data+nrf_rxfifo.Buf_Start)>=nrf_rxfifo.Max_Date)	addr=nrf_rxfifo.P_buf+nrf_rxfifo.Value_Data+nrf_rxfifo.Buf_Start-nrf_rxfifo.Max_Date;
		else	addr=nrf_rxfifo.P_buf+nrf_rxfifo.Value_Data+nrf_rxfifo.Buf_Start;
		*(addr)=temp;
		nrf_rxfifo.Value_Data++;
	}
}
//接收数据到FIFO
void	NRF24L01_Recvfifo(void)
{
	unsigned	char	byte_ctr;
	DYNPD_LEN=NRF24L01_Read_Reg(R_RX_PL_WID_CMD);
	if(DYNPD_LEN>32)
	{
		NRF24L01_RxMode();	//重新初始化接收模式
	}
	else
	{
	SPI_SS = 0;
	SPI_WriteRead(NRF24L01_RD_RX_PLOAD);
	for(byte_ctr=0;byte_ctr<DYNPD_LEN;byte_ctr++)
	NRF_RXFIFO_Write(SPI_WriteRead(0x00));
	SPI_SS = 1;
	}
	NRF24L01_FLAG &= ~NRF24L01_RECIVE;//清除有数据标志
}

//NRF24L01发送fifo数据
void NRF24L01_Sendfifo(unsigned char len)
{
	unsigned char byte_ctr;
	if(NRF24L01_FLAG & NRF24L01_MODE_RX)
	{
		NRF24L01_TxMode();
	}
	NRF24L01_FLAG |= NRF24L01_BUSY;	//BUSY标志置1
	NRF24L01_FLAG &= ~NRF24L01_TX_ACCESS;//清除发送成功标志
	NRF24L01_CE = 0;
	if(NRF24L01_FLAG & NRF24L01_AUTO_ACK)
		{
		//写待发数据,需要回应
		SPI_SS = 0;
		SPI_WriteRead(NRF24L01_WR_TX_PLOAD);
		for(byte_ctr=0; byte_ctr<len; byte_ctr++) 
		SPI_WriteRead(UART_BUF_Read());
		SPI_SS = 1; 
		}
	else
	{
		//写待发数据包,无须回应
		SPI_SS = 0;
		SPI_WriteRead(W_TX_PAYLOAD_NOACK_CMD);
		for(byte_ctr=0; byte_ctr<len; byte_ctr++) 
		SPI_WriteRead(UART_BUF_Read());
		SPI_SS = 1; 
	}
	NRF24L01_CE = 1; 
}