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
/*
#pragma vector=T1_VECTOR   //��ʱ��1�жϺ���
__interrupt void Timer1(void){
  //if(counter < 500)counter++;
  //else{
  //  counter = 0;
    P0_0=~P0_0;
  //  P2_0=~P2_0;
  //}
}
*/
#pragma vector=T1_VECTOR   //��ʱ��1�жϺ���
__interrupt void Timer1(void){
  if(counter < 4)
    counter++;
  else{
    counter = 0;
    P0_0=~P0_0;
  }
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
  T1CC0L = 0xD4;   
  T1CC0H = 0x30;   //�Ƚ�ֵ
  T1CTL = 0x33;   //ͨ��0������Ƶ��up/downģʽ
  EA = 1;     //�����ж�
  T1IE = 1;    //����ʱ��T1�ж�
  while(1){
    P2_0=~P2_0;
    delay();
  }
} // end of main()

