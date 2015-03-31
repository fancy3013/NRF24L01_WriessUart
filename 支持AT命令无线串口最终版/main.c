#include	<STC/STC15F2K60S2.H>
#include "uart.h"
#include	"NRF24L01.H"
#include "delay.h"
#include	"ATCMD.H"

void main()
{
	SPI_Init();
	NRF24L01_Init();
	InitUART();
	NRF24L01_Check();
	 if(~(NRF24L01_FLAG & NRF24L01_CHECK))
	 {
		 	#if BK2423
			NRF24L01_CE=0;
			NRF24L01_Write_Reg(ACTIVATE_CMD, 0x73);
			NRF24L01_CE=1;
			#endif
		 NRF24L01_Check();
	 }
	Timer0Init();
 if(NRF24L01_FLAG & NRF24L01_CHECK)
 {
	 RED=0;
	 delay_ms(200);
	 RED=1;
 }
 else
 {
	 BLUE=0;
	 delay_ms(200);
	 BLUE=1;
 }

  while(1)
	{
		AT_CMD();
		
		if(nrf_rxfifo.Value_Data)
		{
			SendData(NRF_RXFIFO_Read());
		}
		if(uart_buf.Value_Data>=32)
		{
			while(NRF24L01_FLAG & NRF24L01_BUSY);
			NRF24L01_Sendfifo(32);
		}
		else	if(TF0	&&	(uart_buf.Value_Data>0))
		{
			while(NRF24L01_FLAG & NRF24L01_BUSY);
			NRF24L01_Sendfifo(uart_buf.Value_Data);
		}
	}
}
