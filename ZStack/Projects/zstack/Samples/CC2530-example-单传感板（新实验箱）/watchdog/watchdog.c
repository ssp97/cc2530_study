#include "ioCC2530.h"     

unsigned int counter;

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

void Init_Watchdog(void)
{
  WDCTL = 0x00;       //ʱ����һ�룬���Ź�ģʽ
  WDCTL |= 0x08;      //�������Ź�
}
void WatchDog(void)  //ι������
{
  WDCTL = 0xa0;
  WDCTL = 0x50;
}

void main( void )
{
  P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P0_0 = 1; 
  P2_0 = 1;       //Ϩ��LED
  counter = 0;
  CLKCONCMD &= ~0x40;          //ѡ��32M����
  while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
  CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  Init_Watchdog();
  EA = 1;     //�����ж�
  delay();
  delay();
  delay();
  while(1){
    //WatchDog();   //ι��
    delay();
    P0_0 = 0;
    P2_0 = 0;
  }
} // end of main()

