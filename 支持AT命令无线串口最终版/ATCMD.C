/****************************************
AT指令解析引擎
at_scan函数用于监测AT开头,并存储一条AT数据,此例放到串口接收中断
at_cmp函数用于解析存储的AT数组,并执行相应命令
结构体部分位AT指令描述以及函数的对应关系
制作人:Fancy 
QQ:652774317
******************************************/
#include	"ATCMD.H"


code	unsigned char	at_head[AT_HEAD_LEN]={'A','T','+'};	//AT指令开头
unsigned char	addr_cmp=0;		//AT指令开头校验位置
unsigned char	at_addr=0;		//AT缓冲区最后一个数据位置
bit	at_head_flag=0;			//检测AT指令开头标志位
unsigned char at_buf[AT_BUF_MAX]={0};


code	AT_STRUCT	at_array[AT_CMD_COUNT]=
{
	{"CH=",3,3,&AT_CH},
	{"ACK=",4,2,&AT_ACK},
	{"?=",2,0,&AT_SEND}
};
//监测AT开头并将AT命令存储到缓冲区
void	at_scan(void)
{
		if(at_head_flag)
			if(at_addr<AT_BUF_MAX)		at_buf[at_addr++]=SBUF;
			
			if(SBUF==at_head[addr_cmp])				addr_cmp++;
			else	addr_cmp=0;
			if(addr_cmp==AT_HEAD_LEN)
				{
					at_head_flag=1;
					addr_cmp=0;
					at_addr=0;
				}
			
}			
//比较两个字符串cont字节,相同为1,否则返回0			
unsigned char at_cmp(unsigned char *s,unsigned char count)  
{
    unsigned char res;
    for(res=0;res<count;res++)
		{
			if(*(s+res)	!=	at_buf[res])		return	0;
		}
		return	1;  
}
//AT指令解析后并执行相应函数
void	AT_CMD(void)
{
	unsigned char	i;
	if(at_head_flag)
		{
		for(i=0;i<AT_CMD_COUNT;i++)
			{
				if(at_addr	>=	(at_array[i].cmd_len+at_array[i].data_len))
				{
				if(at_cmp(at_array[i].s,at_array[i].cmd_len))
					{
						at_head_flag=0;
						(*at_array[i].Subs)();
					}
				}
			}
		}
}

void	AT_CH(void)
{
	unsigned char	chanel;
	unsigned char	point;
	point=at_array[0].cmd_len;
	chanel=(at_buf[point]-0x30)*100	+	(at_buf[point+1]-0x30)*10	+	(at_buf[point+2]-0x30);
	NRF24L01_Change_Ch(chanel);
}

void	AT_ACK(void)
{
	unsigned char	point;
	point=at_array[1].cmd_len;
	if(at_buf[point]	==	'1')	NRF24L01_FLAG |= NRF24L01_AUTO_ACK;
	if(at_buf[point]	==	'0')	NRF24L01_FLAG &= ~NRF24L01_AUTO_ACK;
}

void	AT_SEND(void)
{
	if(NRF24L01_FLAG & NRF24L01_AUTO_ACK)		SendString("ACK");
	else	SendString("NOACK");
	SendString("CH=");
	SendData(RF_CH/100+0x30);
	SendData(RF_CH%100/10+0x30);
	SendData(RF_CH%10+0x30);
}