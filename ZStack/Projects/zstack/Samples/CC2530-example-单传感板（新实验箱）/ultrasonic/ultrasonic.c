#include "ioCC2530.h"     

unsigned char counter;
unsigned char buf[3];
float distance;

void SendASignal(void)
{
  unsigned char i;
  P1 &= ~(1<<0);
  P1 |= (1<<0);  
  for(i=0;i<33;i++){
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }
  P1 &= ~(1<<0);
} 
void delay(void)
{
  unsigned int i;
  unsigned char j;

  for(i=0;i<5000;i++)
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
}

#pragma vector=T1_VECTOR   //��ʱ��1�жϺ��� 50us
__interrupt void Timer1(void){
    counter++;
    //P0_0=~P0_0;
}

void main( void )
{
  P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P1SEL &= ~((1<<0)|(1<<1));//P1.0,P1.1ΪGPIO        
  P1DIR |= ((1<<0));        //P10ΪOUTPUT
  P1DIR &= ~(1<<1);         //P11ΪINPUT
  P1 &= ~(1<<0);

  P0_0 = 1; 
  P2_0 = 1;                    //Ϩ��LED
  counter = 0;
  CLKCONCMD &= ~0x40;          //ѡ��32M����
  while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
  CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  T1CC0L = 6;
  T1CC0H = 0x00;               //�Ƚ�ֵ
  T1CTL = 0x33;                //ͨ��0������Ƶ��up/downģʽ
  PERCFG = 0x00;               //λ��1 P0 ��
  P0SEL = 0x3c;                //P0 ��������
  U0CSR |= 0x80;               //UART ��ʽ
  U0GCR |= 10;                 //baud_e = 10;
  U0BAUD |= 216;               //��������Ϊ57600
  UTX0IF = 1;
  U0CSR |= 0X40;               //�������
  IEN0 |= 0x84;                //�����жϣ������ж�   
  EA = 1;                      //�����ж�
  T1IE = 1;                    //����ʱ��T1�ж�
  
  while(1){        
    P2_0=~P2_0;
    SendASignal();
    while(1){
      if(P1_1==1) break;
    }
    counter = 0;
    
    while(1){
      if(P1_1==0) break;
    }
    distance = counter;
    distance = (distance*50/1000000*340/2*100);
    counter = (unsigned int)distance;
    //counter����ߵ�ƽ���ȣ���λΪ50us
    buf[0] = counter/100;
    buf[1] = (counter%100)/10;
    buf[2] = (counter%10);
    if(buf[0] > 0x9)
      buf[0] = buf[0] - 0xA + 'A';
    else
      buf[0] = buf[0] + '0';		      
    if(buf[1] > 0x9)
      buf[1] = buf[1] -0xA + 'A';
    else
      buf[1] = buf[1] + '0';
    if(buf[2] > 0x9)
      buf[2] = buf[2] -0xA + 'A';
    else
      buf[2] = buf[2] + '0';
    UartTX_Send_String("Ultrasonic = ",13);
    UartTX_Send_String(&buf[0],1);
    UartTX_Send_String(&buf[1],1);
    UartTX_Send_String(&buf[2],1);
    UartTX_Send_String("cm",2);
    UartTX_Send_String("\r\n",2);
    delay();
  }
} // end of main()

