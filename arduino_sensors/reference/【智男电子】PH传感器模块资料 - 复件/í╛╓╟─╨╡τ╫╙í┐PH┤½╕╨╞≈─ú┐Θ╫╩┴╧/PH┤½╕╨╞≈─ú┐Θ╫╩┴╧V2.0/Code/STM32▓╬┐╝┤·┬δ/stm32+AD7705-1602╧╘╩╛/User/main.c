/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * AD7705:VCC -> 5V; GND -> GND; CS-> PA0;DRDY-> PA1;SLCK -> PA5; DOUT -> PA6; DIN ->PA7;
 * LCD1602:RS -> PA11; RW -> PA12; E -> PA15;
 *         D0~D7 -> PB3~PB10
 * PHģ��: VCC -> 5V; GND -> GND; PO -> AIN1+;
 *
 * ��������������PHֵҺ����ʾ��
             ���ڽ��ղ������õ�PHֵ��������115200����
 *           ��������λ����ʾPHֵ��

**********************************************************************************/
 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include <string.h>
#include "delay.h"
#include "driver_1602.h"
#include "math.h"
#include "AD7705.h"

GPIO_InitTypeDef  GPIO_InitStructure; 
unsigned long PH_num=0,PU_V=0;
float PH_Value=0;
u8 ph_temp=0;
u16 ph_result=0;
u16 adc_1;
float adc_v_1;


unsigned char  Tx[5];   //���߷��ͻ���


/***************************************************************************
 * ��  �� : MAIN����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
 
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO  Clock */					 		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1,ENABLE);
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);		//����������ΪJTAG�ڵ�GPIO��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//����PB����IO��JTAG����
	
	
//1602 D0~D7 ����	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
	GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*I/O ���� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; /*I/O ����ٶ�*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
//1602 EN RS RW ����	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*I/O ���� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; /*I/O ����ٶ�*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
   /* Configure AD7705  DRDY */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void PH_Value_Conversion()
{
	 	TM7705_Init(1);	
 		adc_1 = ReadAD7705(1);
		adc_v_1 = adc_1*5.0/65535;
	
	  PH_Value=-5.887*adc_v_1+21.677;//ʹ��5V���Թ�ʽ
	
    
	if(PH_Value<=0.0){PH_Value=0.0;}
	if(PH_Value>=14.0){PH_Value=14.0;}
	
		  /*��ʾ��ѹ*/
	Tx[0]=(int)(PH_Value*100)/1000+'0';	
	Tx[1]=(int)(PH_Value*100)%1000/100+'0';
	Tx[2]='.';
	Tx[3]=(int)(PH_Value*100)%100/10+'0';
	Tx[4]=(int)(PH_Value*100)%10+'0';	  
	
}

void Display_Data()
{
		//��ʾPHֵ	
		LCD_printchar(3,0,Tx[0]);
		LCD_printchar(4,0,Tx[1]);
		LCD_printchar(5,0,Tx[2]);
		LCD_printchar(6,0,Tx[3]);
		LCD_printchar(7,0,Tx[4]);
			
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	
		GPIO_Configuration();
    /* ����USART1 */
    USART1_Config();
		LCD_init(); 
		AD7705_AllInit();
	
     
	LCD_printstring(0,0,"PH:");
	
  while(1)
	{	
		PH_Value_Conversion();	
		Display_Data();
			
		printf("%s",Tx);
		
	}	
}
/*********************************************END OF FILE**********************/
