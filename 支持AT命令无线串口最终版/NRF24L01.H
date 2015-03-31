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

#ifndef __NRF24L01_H_
#define __NRF24L01_H_

#define	RED		P36
#define	BLUE	P37

#define BK2423 0	//若使用BK2423使能,否则置0
//*********************************************NRF24L01***********************************************************
#define NRF24L01_ADR_LEN        (5)   	//NRF24L01收发地址宽度
#define NRF24L01_TX_PLOAD_LEN   (32)  	//uints TX payload
#define NRF24L01_RX_PLOAD_LEN   (32)  	//uints RX payload
#define	NRF24L01_MODE_RX				(1<<7)
#define	NRF24L01_BUSY						(1<<6)
#define	NRF24L01_TX_ACCESS			(1<<5)
#define	NRF24L01_CHECK					(1<<4)
#define	NRF24L01_RECIVE					(1<<3)
#define NRF24L01_AUTO_ACK				(1<<2)
#define NRF24L01_AUTO_RX				(1<<1)
extern	unsigned char RF_CH;		//RF频道
extern	unsigned char NRF24L01_FLAG;	//NRF24L01当前状态标志
extern	unsigned char TX_ADDRESS[NRF24L01_ADR_LEN];	//本地地址
extern	unsigned char RX_ADDRESS[NRF24L01_ADR_LEN];	//接收地址
extern	unsigned char DYNPD_LEN;			//用于存放动态数据包长度
//***************************************NRF24L01寄存器指令*******************************************************
#define ACTIVATE_CMD						 0x50			//写入0x73激活R_RX_PL_WID,W_ACK_PAYLOAD,W_TX_PAYLOAD_NOACK寄存器,在次写入禁用.只能在省电模式和掉电模式执行
#define W_TX_PAYLOAD_NOACK_CMD	 0xb0			//应用于发送模式，无需应答，需要将EN_DYN_ACK置1有效
#define W_ACK_PAYLOAD_CMD				 0xa8			//适用于接收方，通过PIPE PPP将数据通过ACK形式发送，最多允许三帧数据存在于FIFO
#define R_RX_PL_WID_CMD					 0x60			//读接收到的动态数据包长度（大于32清空RX_FIFO）
#define NRF24L01_READ_REG        0x00  	   // 读寄存器指令
#define NRF24L01_WRITE_REG       0x20 	     // 写寄存器指令
#define NRF24L01_RD_RX_PLOAD     0x61  	   // 读取接收数据指令
#define NRF24L01_WR_TX_PLOAD     0xA0  	   // 写待发数据指令
#define NRF24L01_FLUSE_TX        0xE1 	     // 清除TX FIFO中的数据，应用与发射模式下
#define NRF24L01_FLUSE_RX        0xE2  	   // 清除RX FIFO中的数据，应用与接收模式下
#define NRF24L01_REUSE_TX_PL     0xE3  	   // 定义重复装载数据指令
#define NRF24L01_NOP             0xFF  	   // 无操作，用于读取STATUS寄存器
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define NRF24L01_CONFIG          0x00     // 配置收发状态，CRC校验模式以及收发状态响应方式
#define NRF24L01_EN_AA           0x01       // 自动应答功能设置
#define NRF24L01_EN_RXADDR       0x02       // 可用信道设置
#define NRF24L01_SETUP_AW        0x03       // 收发地址宽度设置
#define NRF24L01_SETUP_RETR      0x04       // 自动重发功能设置
#define NRF24L01_RF_CH           0x05       // 工作频率设置
#define NRF24L01_RF_SETUP        0x06       // 发射速率、功耗功能设置
#define NRF24L01_STATUS          0x07       // 状态寄存器
#define NRF24L01_OBSERVE_TX      0x08       // 发送监测功能
#define NRF24L01_CD              0x09       // 地址检测           
#define NRF24L01_RX_ADDR_P0      0x0A       // 频道0接收数据地址
#define NRF24L01_RX_ADDR_P1      0x0B       // 频道1接收数据地址
#define NRF24L01_RX_ADDR_P2      0x0C       // 频道2接收数据地址
#define NRF24L01_RX_ADDR_P3      0x0D       // 频道3接收数据地址
#define NRF24L01_RX_ADDR_P4      0x0E       // 频道4接收数据地址
#define NRF24L01_RX_ADDR_P5      0x0F       // 频道5接收数据地址
#define NRF24L01_TX_ADDR         0x10       // 发送地址寄存器
#define NRF24L01_RX_PW_P0        0x11       // 接收频道0接收数据长度
#define NRF24L01_RX_PW_P1        0x12       // 接收频道1接收数据长度
#define NRF24L01_RX_PW_P2        0x13       // 接收频道2接收数据长度
#define NRF24L01_RX_PW_P3        0x14       // 接收频道3接收数据长度
#define NRF24L01_RX_PW_P4        0x15       // 接收频道4接收数据长度
#define NRF24L01_RX_PW_P5        0x16       // 接收频道5接收数据长度
#define NRF24L01_FIFO_STATUS     0x17       // FIFO栈入栈出状态寄存器设置
#define NRF24L01_DYNPD			     0x1C				//启用动态数据长度
#define NRF24L01_FEATURE			   0x1D				//特征寄存器
//*****************************************************************************************************************
//NRF24L01状态寄存器值
#define NRF24L01_STATUS_TX_FULL   0x01 //TX FIFO 寄存器满标志。 1 为 满，0为 不满
#define NRF24L01_STATUS_MAX_RT    0x10 //达到最大重发状态
#define NRF24L01_STATUS_TX_DS     0x20 //发送完成中断标志位
#define NRF24L01_STATUS_RX_DR     0x40 //接收到数据中断标志位
#define	NRF24L01_STATUS_RX_FULL		0x02	//RX FIFO寄存器满标志 1 为 满，0为 不满

//NRF24L01引脚定义
#define NRF24L01_CE		P16
#define NRF24L01_IRQ	P32
#define SPI_SS				P12
#define SPI_MOSI			P13
#define SPI_MISO			P14
#define SPI_SCLK			P15

#define NRF_RXFIFO_MAX	120

typedef	struct
{
	unsigned	char	Max_Date;	//缓冲区长度
	unsigned	char	*P_buf;		//缓冲区首地址
	unsigned	char	Value_Data;//有效数据长度
	unsigned	char	Buf_Start;	//起始数据位置
}ST_NRF;
extern	ST_NRF	nrf_rxfifo;
extern	unsigned char	NRF_RXFIFO_Read(void);
extern	void	NRF_RXFIFO_Write(unsigned char temp);
void	NRF24L01_Recvfifo(void);	//读取接收数据到FIFO
void NRF24L01_Sendfifo(unsigned char len);

unsigned	char SPI_WriteRead(unsigned char byte);	//向从机发送一个字节数据并返回接收数据
unsigned char NRF24L01_Write_Reg(unsigned char reg, unsigned char value);	//写NRF24L01寄存器，返回状态值
unsigned char NRF24L01_Read_Reg(unsigned char reg);		//读NRF24L01寄存器，返回寄存器值
void NRF24L01_Read_Buf(unsigned char reg, unsigned char *pBuf,unsigned char bytes);	//读取缓冲区数据
unsigned char NRF24L01_Write_Buf(unsigned char reg,unsigned char *pBuf,unsigned char bytes);	//向缓冲区写入数据
void NRF24L01_Init(void);	//NRF24L01初始化函数
void NRF24L01_TxMode(void);	//NRF24L01进入发送模式
void NRF24L01_RxMode(void);	//NRF24L01进入接收模式
//void NRF24L01_SendFrame(unsigned char *temp,unsigned char len);	//NRF24L01发送数据
//void NRF24L01_RecvFrame(unsigned char *temp);										//NRF24L01接收数据
void NRF24L01_Check(void);		//NRF24L01 SPI通信检查
void NRF24L01_Change_Ch(unsigned char ch);	//更改频道
void NRF24L01_Handler(void);	//NRF24L01中断处理
void SPI_Init(void);

#endif