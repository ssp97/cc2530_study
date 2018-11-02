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

void delay(void);  

unsigned char seg7table[16] = {
    /* 0       1       2       3       4       5       6      7*/
    0xc0,   0xf9,   0xa4,   0xb0,   0x99,   0x92,   0x82,   0xf8,
    /* 8       9      A        B       C       D       E      F*/
    0x80,   0x90,   0x88,   0x83,   0xc6,   0xa1,   0x86,   0x8e };
unsigned char i;

void SegInit()
{
        P1INP &= ~(0x1<<2);          //P1.2上拉
        P1SEL &= ~((1<<0)|(1<<1)|(1<<2));//P1.0,P1.1,P1.2为GPIO
        P1DIR |= (1<<0)|(1<<1)|(1<<2);   //P1.0,P1.1,P1.2为OUTPUT
        P0SEL &= ~(1<<4);       //P0.4为GPIO
        P0DIR |= (1<<4);        //P0.4为OUTPUT
       //74HC595初始化
        P1 |= (0x1<<0); //P1.0 CLR, 复位
        P1 |= (0x1<<1); //P1.1 DATA,数据寄存器
        P1 |= (0x1<<2); //P1.2 CLK，位移寄存器时钟输入
        P0 |= (0x1<<4); //P0.4 LOCK，数据寄存器开始时钟
}

/* 数码管的控制由以下三个寄存器，采用SPI方式控制。
      数据寄存器 DATA
      数据寄存器开始时钟 CLK
      位移寄存器时钟    LOCK
*/
void SendSeg7(unsigned char value){
    unsigned char i;
    P0 &= ~(0x1<<4); //P0.4 LOCK
    P1 &= ~(0x1<<2); //P1.2 CLK
    P1 &= ~(0x1<<1); //P1.1 DATA
    //将数据value的8个bit按串行方式分时赋值P1.1，P1.2控制时钟值高时读取P1.1
    for(i = 0;i < 8;i++)
    {
	if(value&0x80)
	{
	    P1 |= (0x1<<1);//P1.1值1，DATA
	}
	else
	{
	    P1 &= ~(0x1<<1);//P1.1值0，DATA
	}
        value <<=1;
        P1 &= ~(0x1<<2); //P1.2 CLK
        P1 |= (0x1<<2); //P1.2 CLK
    }
    P0 &= ~(0x1<<4); //P0.4 LOCK
    P0 |= (0x1<<4); //P0.4 LOCK
}

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
   SegInit();   //初始化数码管IO口
   
   while(1)
   {
      for(i=0;i<0x10;i++){
        SendSeg7(seg7table[i]);//循环显示0-F
        delay();   // 延时
      }
      SendSeg7(0xff); //清除数码管数据
      delay();   //延时
    }
}

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
  unsigned int j;

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
// the end

