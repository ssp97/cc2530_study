#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

#include "ioCC2530.h"  

#define	    SHT1DATA_HIGH   P1 |= 0x02
#define	    SHT1DATA_LOW    P1 &= 0xFD

#define	    SHT1SCK_HIGH    P1 |= 0x04
#define	    SHT1SCK_LOW	    P1 &= 0xFB

#define     SHT1READY       ((P1>>1)&0x1)
/*****************************************************************************
 ��������
*****************************************************************************/
void Sensor_PIN_INT(void);
uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);
void SHT1_Reset(void);
void SHT1_Start(void);
void SHT1_SendAck(void);
void SHT1_WriteCommandData(uint8);
uint8 SHT1_ReadData(void);
uint8 SHT1_Ready(void);
void SHT1_WriteReg(uint8);
uint16 SHT1_ReadReg(void);
void SHT1_INT(void);
uint32 ReadSHT1(uint8 Addr);
uint8 ReadTc77(void);
void SET_ADC_IO_SLEEP_MODE(void);
void SET_ADC_IO_ADC_MODE(void);
extern void UartTX_Send_String(uint8 *Data,int len);
uint8 CRC8(uint8 crc, uint8 data);
void Delay(void);
void Delay10ms(uint8 Times);
/*��������:����AD�ڵ�����
�������:ChannelNum:�ɼ���ͨ����  0-0xF
                    1000: AIN0�CAIN1
                    1001: AIN2�CAIN3
                    1010: AIN4�CAIN5
                    1011: AIN6�CAIN7
                    1100: GND
                    1101: Reserved
                    1110: Temperature sensor
                    1111: VDD/3
         DecimationRate:�ֱ���  00: 64 decimation rate (7 bits ENOB)
                    01: 128 decimation rate (9 bits ENOB)
                    10: 256 decimation rate (10 bits ENOB)
                    11: 512 decimation rate (12 bits ENOB)
RefVoltage:�ο���ѹ:00: Internal reference
                    01: External reference on AIN7 pin
                    10: AVDD5 pin
                    11: External reference on AIN6�CAIN7 differential input
����ֵ:16bit�Ĳɼ�����
*/
uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage)
{ 
  uint16 AdValue;
  if(ChannelNum == 0xe){//Ƭ���¶ȵ�ADC_SOC
    TR0 = 1;
    ATEST = 1;
  }
  else{
    TR0 = 0;
    ATEST = 0;
  } 
 
  ADCCON3 = ChannelNum&0xf;
  ADCCON3 = ADCCON3 | ((DecimationRate&0x3)<<4);
  ADCCON3 = ADCCON3 | ((RefVoltage&0x3)<<6);   
  ADCCON1 = ADCCON1 | (0x3<<4);//ADCCON1.ST = 1ʱ����
  AdValue = ADCL; //���EOC 
  AdValue = ADCH; 
  ADCCON1 = ADCCON1 | (0x1<<6);//����ת��
  while(!(ADCCON1&0x80));
  AdValue = ADCH;
  AdValue = (AdValue<<6) + (ADCL>>2);
  ADCCON1 =  ADCCON1 & 0x7f;
  return AdValue;
}
/*****************************************************************************
  void Sensor_PIN_INT(void)

  ��������ADC I/O�ڳ�ʼ��.
*****************************************************************************/
void Sensor_PIN_INT(void)
{	  
    //������ʪ�Ȳ���
    P1INP &= (~(0x1 | (0x1<<1) | (0x1<<2) | (0x1<<6) | (0x1<<7)));//P1.0,P1.1,P1.2,P1.6,P1.7����            
    P1SEL &= ~((1<<1)|(1<<2));//P1.1,P1.2ΪGPIO        
    P1DIR |= (1<<1)|(1<<2);//P1.1,P1.2ΪOUTPUT
}

//��uC��SHT10ͨ���ж�ʱ,��λͨ�ſ�
void SHT1_Reset(void)
{
  uint8 i;
  
  SHT1DATA_HIGH;
  for(i=0;i<11;i++){
    Delay();
    SHT1SCK_LOW;
    Delay();
    SHT1SCK_HIGH;  
    Delay();
  }
}

//������ʼ�ź�
void SHT1_Start(void)
{
  SHT1DATA_HIGH;
  SHT1SCK_LOW;
  Delay();
  SHT1SCK_HIGH;
  Delay();
  SHT1DATA_LOW;
  Delay();
  SHT1SCK_LOW;
  Delay();
  SHT1SCK_HIGH;
  Delay();
  SHT1DATA_HIGH;  
  Delay();
  SHT1SCK_LOW;
}

//Ϊ0ʱ,д������ȷ;Ϊ1ʱ����
uint8 SHT1_Ready(void)
{  
  //��Ӧ���ź�    
  P1DIR &= ~(1<<1);   //P11ΪINPUT 
  Delay();  
  return(SHT1READY);
}

void SHT1_SendAck(void)
{  
  SHT1SCK_HIGH;    
  Delay();
  SHT1SCK_LOW;    
  Delay();
}
//Ϊ0ʱ,д������ȷ;Ϊ1ʱ����
void SHT1_WriteCommandData(uint8 bCommand)
{
  uint8 i;
  Delay();  
  SHT1SCK_LOW;
  Delay();
  for(i=0;i<8;i++){
    if(bCommand&(0x1<<(7-i)))
      SHT1DATA_HIGH;
    else
      SHT1DATA_LOW;
    Delay();
    SHT1SCK_HIGH;
    Delay();
    SHT1SCK_LOW;    
  }  
  P1DIR &= ~(1<<1);   //P11ΪINPUT 
}

//��һ���ֽڵ�����
uint8 SHT1_ReadData(void)
{
  uint8 i,bResult;
  bResult = 0;
  P1DIR &= ~(1<<1);   //P11ΪINPUT   
  Delay();
  for(i=0;i<8;i++){       
    if(SHT1_Ready() != 0)
      bResult |= (0x1<<(7-i)); 
    Delay();
    SHT1SCK_HIGH;
    Delay();
    SHT1SCK_LOW;
  }
  P1DIR |= (1<<1);    //P11ΪOUTPUT   
  return bResult;
}

//д״̬�Ĵ���
void SHT1_WriteReg(uint8 Value)
{
  while(1){    
    SHT1_Start();  
    SHT1_WriteCommandData(6);
    if(SHT1_Ready() != 0){  //��Ӧ��
      SHT1_Reset();  
      continue;
    }
    else{      
      P1DIR |= (1<<1);    //P11ΪOUTPUT  
      SHT1DATA_LOW;
      SHT1_SendAck();    
    }
    SHT1_WriteCommandData(Value);
    if(SHT1_Ready() != 0){  //��Ӧ��
      SHT1_Reset(); 
      continue;
    }
    else{      
      SHT1DATA_LOW;
      SHT1_SendAck();   
    }
    break;
  }   
}

uint16 SHT1_ReadReg(void)
{
  uint16 lResult;
  while(1){     
    SHT1_Start();  
    SHT1_WriteCommandData(7);
    if(SHT1_Ready() != 0){  //��Ӧ��
      SHT1_Reset();        
      continue;      
    }
    else{      
      SHT1DATA_LOW;
      SHT1_SendAck();   
      break;
    }    
  } 
  
  lResult = (SHT1_ReadData()<<8);  
  SHT1DATA_LOW;
  SHT1_SendAck();
      
  lResult |= SHT1_ReadData(); 
  SHT1DATA_HIGH;
  SHT1_SendAck();
    
  return lResult;
}

uint32 ReadSHT1(uint8 Addr)
{
  uint32 lResult;
  while(1){        
    SHT1_Start();              
    SHT1_WriteCommandData(Addr);
    
    if(SHT1_Ready() != 0){  //��Ӧ��
      SHT1_Reset();  
      continue;      
    }
    else{
      SHT1_SendAck();
      break;
    }    
  }
  Delay10ms(60);
  lResult = SHT1_ReadData();  
  lResult = lResult<<16;
  SHT1DATA_LOW;
  Delay();
  SHT1_SendAck();
  
  lResult |= ((uint16)SHT1_ReadData()<<8); 
  SHT1DATA_LOW;
  Delay();
  SHT1_SendAck();
    
  lResult |= SHT1_ReadData(); 
  SHT1DATA_HIGH;
  Delay();
  SHT1_SendAck();
  
  return lResult;
}

uint8 CRC8(uint8 crc, uint8 data)
{
  uint8 i;
  crc = crc ^data;
  for (i = 0; i < 8; i++)
  {
     if ((crc & 0x01) != 0) crc = (crc >> 1) ^ 0x8c;
     else crc = crc >> 1;
  }
  return crc;
}

void Delay(void)
{
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");    
}

void Delay10ms(uint8 Times){
    uint8 i;
    uint16 j;    
    for(i=0;i<Times;i++){
      for(j=0;j<5000;j++){        
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
   }
}