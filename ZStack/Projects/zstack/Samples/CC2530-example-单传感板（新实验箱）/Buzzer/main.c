/*******************************************************************************
*  OSC = Default (reset)  16MHz RC 
********************************************************************************
* 编译器：
* IAR Assembler for 8051 7.51A/W32 
* IAR C/C++ Compiler for 8051  7.51A
*
* 文 件 名:     main.c
* 目标器件:     cc2530
* 编    写:     DaTai ban
* 修    订:     2010-9-15
* 版    本:     1.0
* 描    述:
*         DTD253_EMK. 开发板例程。
*         让开发板上的四个LED闪烁。
* 
*******************************************************************************/

#include "ioCC2530.h"      // 申明该文件中用到的头文件
unsigned char pluse_table_1[] = 
{
	0x05, 0x09, 0x0a, 0x06,
};
void delay(void); 
/******************************************************************************
* @fn  main
*
* @brief
*      Main function of application example. Flash 4 LEDs
*
* Parameters:
* @param  void
* @return void
******************************************************************************/
void main( void )
{
  P0DIR = (0x01<<0)|(0x01<<5);  //P0.0,P0.5为输出方式
  P2DIR |= 0x01;  // 设置P2.0为输出方式
  
  while(1)
  { 
    P0_5 = 1;//打开蜂鸣器 
    P0_0 = 0;
    P2_0 = 0;
    delay();
    P0_5 = 0;//关闭蜂鸣器 
    P0_0 = 1;
    P2_0 = 1;    
    delay();    
  }			
} // end of main()

/******************************************************************************
* @fn  delay
*
* @brief
*      延时函数.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void delay(void)   //延时时间约400ms
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
  // the end

