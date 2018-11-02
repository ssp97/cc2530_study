#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int 

char uart_buffer;
unsigned int adc_value; 
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

uint16 ReadAdValue(uint8 SREF,uint8 SDIV,uint8 Channel)
{  
  uint16 AdcValue;
  ADCCON3 = (SREF<<6)|(SDIV<<4)|Channel;  
  ADCCON1 &= 0x30; //ֹͣA/D
  ADCL &= 0x00; //EOC����
  ADCH &= 0x00; //EOC����
  ADCCON1 |= 0x40; //����A/D��  
  while(!(ADCCON1 & 0x80));          //�ȴ�ADת������ 
  AdcValue = ADCH;
  AdcValue = ((AdcValue<<6)+(ADCL >> 2));
  return AdcValue;
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

void UartTX_Send_Data(char Data,int len)   //���ڷ��ͺ���
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
    //UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
  float temp;
  unsigned char buf[8];
  temp = 0;
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
  while(1){   
    P2_0=~P2_0;    
    if(uart_buffer == 0x01)
    {
      //temp = ReadAdValue(2,3,0xf);
      temp = ReadAdValue(2,3,0x1);
      temp = (temp/0x1fff)*33;
      //UartTX_Send_Data(adc_value,1);
      uart_buffer = 0;    
      buf[0] = (((unsigned char)temp)/10)+'0';
      buf[1] = 46;  
      buf[2] = (((unsigned char)temp)%10)+'0';
      buf[3] = 86;
      buf[4] = '\t';
      UartTX_Send_String(&buf[0],5);
    }
  }
} // end of main()

