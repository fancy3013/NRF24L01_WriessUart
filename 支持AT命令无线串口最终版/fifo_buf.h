#ifndef	fifo_buf_h_
#define	fifo_buf_h_



typedef	struct
{
	unsigned	char	Max_Date;	//����������
	unsigned	char	*P_buf;		//�������׵�ַ
	unsigned	char	Value_Data;//��Ч���ݳ���
	unsigned	char	Buf_Start;	//��ʼ����λ��
}ST_FIFO_BUF;

extern	unsigned char	FIFO_BUF_Read(ST_FIFO_BUF *buf);
extern	void	FIFO_BUF_Write(ST_FIFO_BUF *buf,unsigned char temp);

#endif