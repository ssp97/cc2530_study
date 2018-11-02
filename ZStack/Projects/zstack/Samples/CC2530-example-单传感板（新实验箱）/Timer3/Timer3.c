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

#pragma vector=T3_VECTOR   //��ʱ��3�жϺ���
__interrupt void Timer3(void){
  //if(counter < 500)counter++;
  //else{
  //  counter = 0;
    P0_0=~P0_0;
  //  P2_0=~P2_0;
  //}
}

void main( void )
{
  P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P0_0 = 1; 
  P2_0 = 1;       //Ϩ��LED
  counter = 0;
  CLKCONCMD &= ~0x40;          //ѡ��32M����
  while(!(SLEEPSTA & 0x40));   //�ȴ�XOSC�ȶ�
  CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  T3CC0 = 125;                 //�Ƚ�ֵ
  T3CTL = 0x0f;                //����Ƶ�������ֵ��up/downģʽ
  T3CTL |= 0x10;               //������ʱ��
  EA = 1;                      //�����ж�
  T3IE = 1;                    //����ʱ��T3�ж�
  while(1){
    P2_0=~P2_0;
    delay();
  }
} // end of main()

