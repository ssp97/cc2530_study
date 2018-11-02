#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

extern void Sensor_PIN_INT(void);
extern uint32 ReadSHT1(uint8 Addr);

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

void UartTX_Send_String(unsigned char *Data,int len)   //���ڷ��ͺ���
{
    int j;
    for(j=0;j<len;j++)
    {
    	U0DBUF = *Data++;   
    	while(UTX0IF == 0);
    	UTX0IF = 0;
    }
}

void UartTX_Send_Data(unsigned char Data,int len)   //���ڷ��ͺ���
{
    int j;
    for(j=0;j<len;j++)
    {
    	U0DBUF = Data;   
    	while(UTX0IF == 0);
    	UTX0IF = 0;
    }
}

#pragma vector=URX0_VECTOR   //uart0�жϺ���
__interrupt void uart0(void){
    URX0IF = 0; //���жϱ�־
    P0_0=~P0_0;
    uart_buffer = U0DBUF;
    UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
    unsigned long lTemp;
    float RHTValue;
    unsigned char buf[8];
    P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
    P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
    P0_0 = 1; 
    P2_0 = 1;       //Ϩ��LED
    CLKCONCMD &= ~0x40;          //ѡ��32M����
    while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
    CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
    SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
    PERCFG = 0x00;               //λ��1 P0 ��
    P0SEL = 0x3c;                //P0 ��������
    U0CSR |= 0x80;               //UART ��ʽ
    U0GCR |= 10;                 //baud_e = 10;
    U0BAUD |= 216;               //��������Ϊ57600
    UTX0IF = 1;
    U0CSR |= 0X40;               //�������
    IEN0 |= 0x84;                //�����жϣ������ж�  	
    Sensor_PIN_INT();

    UartTX_Send_String("Testing...\r\n",12);
    while(1){ 
	lTemp = ReadSHT1(3);//14bit�¶�
        lTemp = lTemp >> 8;
        RHTValue = lTemp;
        RHTValue = 0.01 * RHTValue - 39.64;
        buf[0] = (uint8)RHTValue;//��ʪ�ȴ������¶� 
	buf[0] = ( ((buf[0]/10)<<4) + (buf[0]%10) );
        buf[1] = (buf[0]>>4)&0xf;
	if(buf[1] > 0x9)
            buf[1] = buf[1] - 0XA + 'A';
	else
	    buf[1] = buf[1] + '0';
	buf[2] = (buf[0])&0xf;
	if(buf[2] > 0x9)
	    buf[2] = buf[2] -0XA + 'A';
	else
	    buf[2] = buf[2] + '0';
	
        lTemp = ReadSHT1(5);//12bitʪ��
        lTemp = lTemp >> 8;
        RHTValue = lTemp;
        RHTValue = 0.0405 * RHTValue -4 - 2.8*RHTValue*RHTValue/1000000;
        buf[3] = (uint8)RHTValue;//��ʪ�ȴ�����ʪ�� 
	buf[3] = ( ((buf[3]/10)<<4) + (buf[3]%10) );
	
	buf[4] = (buf[3]>>4)&0xf;
	if(buf[4] > 0x9)
            buf[4] = buf[4] - 0XA + 'A';
	else
	    buf[4] = buf[4] + '0';
	buf[5] = (buf[3])&0xf;
	if(buf[5] > 0x9)
            buf[5] = buf[5] -0XA + 'A';
	else
	    buf[5] = buf[5] + '0';
        
      	UartTX_Send_String("Temperature = ",14);
      	UartTX_Send_String(&buf[1],1);
	UartTX_Send_String(&buf[2],1);
	UartTX_Send_String("    ",4);
	
	UartTX_Send_String("humidity = ",11);
	UartTX_Send_String(&buf[4],1);
	UartTX_Send_String(&buf[5],1);
	UartTX_Send_String("\r\n",2);
    	
    	delay();
    }
} // end of main()