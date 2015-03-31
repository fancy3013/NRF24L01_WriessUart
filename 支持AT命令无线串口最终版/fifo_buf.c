/******************************************************
环形FIFO缓冲区,定义一个数组作为缓冲区
用以下函数来实现环形存储
By Fancy	QQ:652774317
*****************************************************/
#include	"fifo_buf.h"


unsigned char	FIFO_BUF_Read(ST_FIFO_BUF *buf)
{
	unsigned char	*addr;
	addr=buf->P_buf + buf->Buf_Start;
	if(buf->Value_Data>0)
	{
	buf->Value_Data--;
	buf->Buf_Start++;
	if(buf->Buf_Start >= buf->Max_Date)	buf->Buf_Start=0;
	}
	return	*addr;
}

void	FIFO_BUF_Write(ST_FIFO_BUF *buf,unsigned char temp)
{
	unsigned char *addr;
	if(buf->Value_Data<buf->Max_Date)
	{
		if((buf->Value_Data+buf->Buf_Start)>=buf->Max_Date)	addr=buf->P_buf+buf->Value_Data+buf->Buf_Start-buf->Max_Date;
		else	addr=buf->P_buf+buf->Value_Data+buf->Buf_Start;
		*(addr)=temp;
		buf->Value_Data++;
	}
}