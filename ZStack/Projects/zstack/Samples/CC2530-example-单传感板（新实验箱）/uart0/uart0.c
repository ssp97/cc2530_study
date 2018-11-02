#include "ioCC2530.h"     

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
    //UartTX_Send_String("welcome",10);
    UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
  unsigned char buf[8];
  P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P0_0 = 1; 
  P2_0 = 1;       //Ϩ��LED
  CLKCONCMD &= ~0x40;          //ѡ��32M����
  while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
  CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  PERCFG = 0x00;              //λ��1 P0 ��
  P0SEL = 0x3c;               //P0 ��������
  U0CSR |= 0x80;              //UART ��ʽ
  U0GCR |= 10;                //baud_e = 10;
  U0BAUD |= 216;              //��������Ϊ57600
  UTX0IF = 1;
  U0CSR |= 0X40;              //�������
  IEN0 |= 0x84;               //�����жϣ������ж�
  buf[0] = '\t';
  while(1){
    P2_0=~P2_0;
    delay();
    if(uart_buffer == 0x01){
      UartTX_Send_String("welcome",7);
      UartTX_Send_String(&buf[0],1);
    }
  }
} // end of main()
