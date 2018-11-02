#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

extern void Sensor_PIN_INT(void);
extern uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);

char uart_buffer;

void delay(void)
{
  	unsigned int i;
  	unsigned char j;

  	for(i=0;i<1500;i++)
  	{
    	for(j=0;j<200;j++)
    	{
      		asm("NOP");
      		asm("NOP");
     	 	asm("NOP");
    	}
  	}
} 

void UartTX_Send_String(unsigned char *Data,int len)   //串口发送函数
{
  	int j;
  	for(j=0;j<len;j++)
  	{
    	U0DBUF = *Data++;   
    	while(UTX0IF == 0);
    	UTX0IF = 0;
  	}
}

void UartTX_Send_Data(unsigned char Data,int len)   //串口发送函数
{
  	int j;
  	for(j=0;j<len;j++)
  	{
    	U0DBUF = Data;   
    	while(UTX0IF == 0);
    	UTX0IF = 0;
  	}
}

#pragma vector=URX0_VECTOR   //uart0中断函数
__interrupt void uart0(void){
    URX0IF = 0; //清中断标志
    P0_0=~P0_0;
    uart_buffer = U0DBUF;

    UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
	uint16 AdValue;
  	unsigned char buf[12];  	
	Sensor_PIN_INT();
	
	UartTX_Send_String("Testing...\r\n",12);
  	while(1){    	
		AdValue = ReadAdcValue(0x1,3,2);
        AdValue = AdValue>>6;
        buf[0] = (uint8)AdValue;
        
        AdValue = ReadAdcValue(0x4,3,2);
        AdValue = AdValue>>6;
        buf[3] = (uint8)AdValue;
                  
        AdValue = ReadAdcValue(0x5,3,2);
        AdValue = AdValue>>6;
        buf[6] = (uint8)AdValue;
		
		//X轴加速度处理
		if(buf[0] ==  0x53){
		  	buf[0] = 0;
			buf[9] = 0;
		}  
		else if(buf[0] >  0x53){
		  	buf[0] = buf[0] - 0x53;
			buf[9] = 1;
		}
		else{
		  	buf[0] = 0x53 - buf[0];
			buf[9] = 2;
		}		  
		buf[1] = (buf[0] >> 4)&0xf;	
		if(buf[1] > 0x9)
		  	buf[1] = buf[1] - 0XA + 'A';
		else
			buf[1] = buf[1] + '0';
		
		buf[2] = buf[0]&0xf;			
		if(buf[2] > 0x9)
		  	buf[2] = buf[2] -0XA + 'A';
		else
			buf[2] = buf[2] + '0';
				
		//Y轴加速度处理
		if(buf[3] ==  0x45){
		  	buf[3] = 0;
			buf[10] = 0;
		}  
		else if(buf[3] >  0x45){
		  	buf[3] = buf[3] - 0x45;
			buf[10] = 1;
		}
		else{
		  	buf[3] = 0x45 - buf[3];
			buf[10] = 2;
		}
		buf[4] = (buf[3] >> 4)&0xf;	
		if(buf[4] > 0x9)
		  	buf[4] = buf[4] - 0XA + 'A';
		else
			buf[4] = buf[4] + '0';
		
		buf[5] = buf[3]&0xf;			
		if(buf[5] > 0x9)
		  	buf[5] = buf[5] -0XA + 'A';
		else
			buf[5] = buf[5] + '0';
		
		//Z轴加速度处理
		if(buf[6] ==  0x49){
		  	buf[6] = 0;
			buf[11] = 0;
		}  
		else if(buf[6] >  0x49){
		  	buf[6] = buf[6] - 0x49;
			buf[11] = 1;
		}
		else{
		  	buf[6] = 0x49 - buf[6];
			buf[11] = 2;
		}
		buf[7] = (buf[6] >> 4)&0xf;	
		if(buf[7] > 0x9)
		  	buf[7] = buf[7] - 0XA + 'A';
		else
			buf[7] = buf[7] + '0';
		
		buf[8] = buf[6]&0xf;			
		if(buf[8] > 0x9)
		  	buf[8] = buf[8] -0XA + 'A';
		else
			buf[8] = buf[8] + '0';
		
        UartTX_Send_String("X Accelerometer = ",18);
		if(buf[9] == 0)
		  	UartTX_Send_String(" ",1);
		else if(buf[9] == 1)
		  	UartTX_Send_String("+",1);
		else if(buf[9] == 2)
			UartTX_Send_String("-",1);	  			
      	UartTX_Send_String(&buf[1],2);
		UartTX_Send_String("        ",2);		
		UartTX_Send_String("Y Accelerometer = ",18);		
		if(buf[10] == 0)
		  	UartTX_Send_String(" ",1);
		else if(buf[10] == 1)
		  	UartTX_Send_String("+",1);
		else if(buf[10] == 2)
			UartTX_Send_String("-",1);	  			
      	UartTX_Send_String(&buf[4],2);
		UartTX_Send_String("        ",2);		
		UartTX_Send_String("Z Accelerometer = ",18);
		if(buf[11] == 0)
		  	UartTX_Send_String(" ",1);
		else if(buf[11] == 1)
		  	UartTX_Send_String("+",1);
		else if(buf[11] == 2)
			UartTX_Send_String("-",1);	  			
      	UartTX_Send_String(&buf[7],2);
		UartTX_Send_String("\r\n",2);			
    	
    	delay();
  	}
} // end of main()