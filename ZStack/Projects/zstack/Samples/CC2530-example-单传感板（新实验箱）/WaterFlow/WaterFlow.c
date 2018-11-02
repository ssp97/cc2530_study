#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long
extern void Sensor_PIN_INT(void);
extern uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);
char uart_buffer;
uint16 WaterFlowCount = 0;
uint16 Count1ms = 0;
uint8  f1s = 0;

#pragma vector = 0x007B 
__interrupt void P1_ISR(void)
{ 
   if(P1IFG & (0x1<<6))
      WaterFlowCount++;
   P1IFG = 0;
   P1IF = 0;
}

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

#pragma vector=T1_VECTOR   //��ʱ��1�жϺ��� 1ms
__interrupt void Timer1(void){
    if(++Count1ms > 1000){
        Count1ms = 0;
        f1s = 1;
    }
}

#pragma vector=URX0_VECTOR   //uart0�жϺ���
__interrupt void uart0(void){
    URX0IF = 0; //���жϱ�־
    P0_0=~P0_0;
    uart_buffer = U0DBUF;
    //UartTX_Send_String("welcome",10);
    UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
    unsigned char buf[8];
    P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
    APCFG = (0x1<<1)|(0x1<<4)|(0x1<<6);//P01,P04,P06ΪADC�� 
    P1DIR = (0x01<<3)|(0x01<<4);  //����P1.3,P1_4Ϊ�����ʽ��
    P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
    P0_0 = 1; 
    P2_0 = 1;       //Ϩ��LED
    CLKCONCMD &= ~0x40;          //ѡ��32M����
    while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
    CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
    
    P1SEL |= (1<<6);//P16Ϊ���蹦��       
    PICTL &= ~(1<<2); //P14~P17�����ش���
    P1IEN |= (1<<6);//P16�ж�ʹ��
    IEN2 |= (1<<4);//P1���ж�ʹ��
    P1 &= ~((1<<3)|(1<<4));//00:��,���ܾ���ͨ  
    
    T1CC0L = 125;   
    T1CC0H = 0x00;   //�Ƚ�ֵ
    T1CTL = 0x33;    //ͨ��0������Ƶ��up/downģʽ
    PERCFG = 0x00;               //λ��1 P0 ��
    P0SEL = 0x3c;                //P0 ��������
    U0CSR |= 0x80;               //UART ��ʽ
    U0GCR |= 10;                 //baud_e = 10;
    U0BAUD |= 216;               //��������Ϊ57600
    UTX0IF = 1;
    U0CSR |= 0X40;               //�������
    IEN0 |= 0x84;                //�����жϣ������ж�   
    EA = 1;          //�����ж�
    T1IE = 1;        //����ʱ��T1�ж�            
    
    UartTX_Send_String("Testing...\r\n",12);
    while(1){            
        P2_0=~P2_0;   
        Count1ms = 0;
        f1s = 0;
        WaterFlowCount = 0;
        while(1){
            if(f1s == 1){
                WaterFlowCount = (WaterFlowCount) * 30 / 4;
                buf[0] = (uint8)(WaterFlowCount/10000);
                buf[1] = (uint8)((WaterFlowCount%10000)/1000);
                buf[2] = (uint8)((WaterFlowCount%1000)/100);
                buf[3] = (uint8)((WaterFlowCount%100)/10);
                buf[4] = (uint8)(WaterFlowCount%10);
                if(buf[0] > 0x9)
                    buf[0] = buf[0] - 0XA + 'A';
          	else
	            buf[0] = buf[0] + '0';
                if(buf[1] > 0x9)
                    buf[1] = buf[1] - 0XA + 'A';
        	else
	            buf[1] = buf[1] + '0';
                if(buf[2] > 0x9)
                    buf[2] = buf[2] - 0XA + 'A';
        	else
	            buf[2] = buf[2] + '0';
                if(buf[3] > 0x9)
                    buf[3] = buf[3] - 0XA + 'A';
        	else
	            buf[3] = buf[3] + '0';
                if(buf[4] > 0x9)
                    buf[4] = buf[4] - 0XA + 'A';
        	else
	            buf[4] = buf[4] + '0';
                break;                
            }
        }                
	UartTX_Send_String("WaterFlow = ",12);
      	UartTX_Send_String(&buf[0],1);
        UartTX_Send_String(&buf[1],1);
        UartTX_Send_String(&buf[2],1);
        UartTX_Send_String(&buf[3],1);
        UartTX_Send_String(&buf[4],1);
        UartTX_Send_String("L/MIN",5);    
	UartTX_Send_String("\r\n",2);	
    }
} // end of main()