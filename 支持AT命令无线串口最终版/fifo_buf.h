#ifndef	fifo_buf_h_
#define	fifo_buf_h_



typedef	struct
{
	unsigned	char	Max_Date;	//缓冲区长度
	unsigned	char	*P_buf;		//缓冲区首地址
	unsigned	char	Value_Data;//有效数据长度
	unsigned	char	Buf_Start;	//起始数据位置
}ST_FIFO_BUF;

extern	unsigned char	FIFO_BUF_Read(ST_FIFO_BUF *buf);
extern	void	FIFO_BUF_Write(ST_FIFO_BUF *buf,unsigned char temp);

#endif