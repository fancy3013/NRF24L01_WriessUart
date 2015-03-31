/****************************************
AT指令解析引擎
at_scan函数用于监测AT开头,并存储一条AT数据,此例放到串口接收中断
at_cmp函数用于解析存储的AT数组,并执行相应命令
结构体部分位AT指令描述以及函数的对应关系
制作人:Fancy 
QQ:652774317
******************************************/
#ifndef __ATCMD_H_
#define __ATCMD_H_

#include	<STC/STC15F2K60S2.H>
#include "uart.h"
#include	"NRF24L01.H"

#define	AT_BUF_MAX		7					//AT指令缓冲区空间,不包含AT开头
#define	AT_HEAD_LEN		3					//AT指令头长度
#define	AT_CMD_COUNT	3				//AT指令总数
/************************
AT+CMD=XX
AT+	指令开头
CMD=	命令
XX		数据
*************************/
typedef	struct
{
	unsigned char	*s;		//AT命令行
	unsigned	char	cmd_len;	//命令长度,不含AT开头
	unsigned	char	data_len;	//数据长度
	unsigned char 	(*Subs)();	//命令对应函数
}AT_STRUCT;

extern	code	unsigned char	at_head[AT_HEAD_LEN];	//AT指令开头
extern	unsigned char	addr_cmp;		//AT指令开头校验位置
extern	unsigned char	at_addr;		//AT缓冲区最后一个数据位置
extern	bit	at_head_flag;			//检测AT指令开头标志位
extern	unsigned char at_buf[AT_BUF_MAX];
extern	code	AT_STRUCT	at_array[AT_CMD_COUNT];
//监测AT开头并将AT命令存储到缓冲区
extern	void	at_scan(void);
//比较两个字符串cont字节,相同为1,否则返回0			
extern	unsigned char at_cmp(unsigned char *s,unsigned char count);  
//AT指令解析后并执行相应函数
extern	void	AT_CMD(void);
extern	void	AT_CH(void);
extern	void	AT_ACK(void);
extern	void	AT_SEND(void);
#endif