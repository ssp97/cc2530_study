#include "ioCC2530.h"      

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

void SET_POWER_MODE(unsigned char mode)    //����ʡ��ģʽ,mode:0-3
{ 
  if(mode < 4) 
  { 
    SLEEPCMD &= 0xfc;
    SLEEPCMD |= mode;
    PCON |= 0x01; 
    asm("NOP"); 
  }
  else
    PCON = 0;
}

#pragma vector=P0INT_VECTOR    //�ⲿ�жϺ���
__interrupt void P0_INT(void){
  if(P0IFG > 0){
    P0IFG = 0;  
    unsigned i;
    for(i=0;i<5;i++)
    {
      P0_0 = ~P0_0;
      delay();
    }
  }
  P0IFG = 0;
}

void Init_SLEEPCMD_TIMER(void)
{
  STIE = 1;
  STIF = 0;
}

#pragma vector=ST_VECTOR    //˯���жϺ���
__interrupt void ST_INT(void){
  STIF = 0;
  unsigned i;
    for(i=0;i<5;i++)
    {
      P2_0 = ~P2_0;
      delay();
    }
}

void addToSLEEPCMDTimer(unsigned int sec)
{
  long int SLEEPCMDTimer = 0;
  SLEEPCMDTimer |= ST0;
  SLEEPCMDTimer |= (long int)ST1 << 8;
  SLEEPCMDTimer |= (long int)ST2 << 16;
  SLEEPCMDTimer += ((long int)sec * (long int)32768);
  ST2 = (char)(SLEEPCMDTimer >> 16);
  ST1 = (char)(SLEEPCMDTimer >> 8);
  ST0 = (char) SLEEPCMDTimer;
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

void main( void )
{
  P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P0_0 = 1; 
  P2_0 = 1;   //Ϩ��LED
  P0IEN = 0x10;
  PICTL = 0x11;
  P0IFG = 0;
  P0IE  = 1;
  CLKCONCMD &= ~0x40;          //ѡ��32M����
  while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
  CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  P0IEN = 0x10;
  PICTL = 0x11;
  P0IFG = 0;
  P0IE  = 1;
  EA = 1;
  Init_SLEEPCMD_TIMER();
  //SET_POWER_MODE(2);   //����PM2ʡ��ģʽ
  while(1)            //�ȴ�˯�߶�ʱ���жϻ���
  {
    addToSLEEPCMDTimer(5);  //5�뻽��һ��
    SET_POWER_MODE(2);   //����PM2ʡ��ģʽ
  }
} // end of main()


