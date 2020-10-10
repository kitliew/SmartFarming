/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * 文件名  ：main.c
 * 描述    ：        
 * 硬件连接：
 * LCD1602:RS -> PA11; RW -> PA12; E -> PA15;
 *         D0~D7 -> PB3~PB10
 * EC传感器模块: VCC -> 5V; GND -> GND; AO -> PA2;
 *
 * 温度传感器模块: VCC -> 3.3V; GND -> GND; DO -> PB12;
 *
 * 功能描述：测量EC值液晶显示(ADC1、PA2、DMA方式读取)；
             串口接收测量所得的EC值（波特率115200）；
 *           可连接上位机显示EC值；
 * 作者    ：WAAAX电子
 * 淘宝    ：https://ILOVEMCU.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include <string.h>
#include "delay.h"
#include "driver_1602.h"
#include "math.h"
#include "bsp_adc.h"
#include "ds18b20.h"

#define RES2 820.0   //运放电阻，与硬件电路有关
#define ECREF 200.0 //极片输入电压，与硬件电路相关

GPIO_InitTypeDef  GPIO_InitStructure; 
unsigned char AD_CHANNEL=0;
float EC_voltage;
float EC_value=0.0,voltage_value;
float temp_data=250;
float compensationCoefficient=1.0;//温度校准系数
float compensationVolatge;
float kValue=1.0;
float kValue_Low=1.0;  //校准时进行修改
float kValue_High=1.0; //校准时进行修改
float rawEC=0.0;
float EC_valueTemp=0.0;

u8 ISendByte(unsigned char sla,unsigned char c);
u8 IRcvByte(unsigned char sla);	
u8 SPIx_ReadWriteByte(u8 TxData);

unsigned char  Tx[20];   //无线发送缓存


// ADC1转换的电压值通过DMA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal;  

/***************************************************************************
 * 描  述 : MAIN函数
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
 
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO  Clock */					 		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1,ENABLE);
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);		//屏蔽所有作为JTAG口的GPIO口
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//屏蔽PB口上IO口JTAG功能
	
	
//1602 D0~D7 引脚	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
	GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*I/O 方向 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; /*I/O 输出速度*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
//1602 EN RS RW 引脚	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*I/O 方向 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; /*I/O 输出速度*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
   /* Configure  DRDY */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}


/**************TDS值采集函数***************/
void EC_Value_Conversion()
{
	EC_voltage =(float) ADC_ConvertedValue/4096*3300; // 读取转换的AD值
	
	rawEC = 1000*EC_voltage/RES2/ECREF;
	EC_valueTemp=rawEC*kValue;
	
	/*First Range:(0,2); Second Range:(2,20)*/
	if(EC_valueTemp>2.0)
	{
	  kValue=kValue_High;
	}
	else if(EC_valueTemp<=2.0)
	{
	  kValue=kValue_Low;
	}
	EC_value=rawEC*kValue;
	
	compensationCoefficient=1.0+0.0185*((temp_data/10)-25.0); 
	
	EC_value=EC_value/compensationCoefficient;
	
		if((EC_value<=0)){EC_value=0;}
		if((EC_value>20)){EC_value=20;}//20mS/cm
		
		/*显示EC*/
		Tx[0]=(int)(EC_value*100)/1000+'0';
		Tx[1]=(int)(EC_value*100)%1000/100+'0';
		Tx[2]='.';
		Tx[3]=(int)(EC_value)%100/10+'0';	
		Tx[4]=(int)(EC_value)%10+'0';
		
		LCD_printchar(0x3,0,Tx[0]);
		LCD_printchar(0x4,0,Tx[1]);
		LCD_printchar(0x5,0,Tx[2]);
		LCD_printchar(0x6,0,Tx[3]);
		LCD_printchar(0x7,0,Tx[4]);
}

void TEMP_Value_Conversion()
{
	  temp_data=DS18B20_Get_Temp();
	
	  Tx[5]=(int)(temp_data)%1000/100+'0';	
	  Tx[6]=(int)(temp_data)%100/10+'0';
	  Tx[7]='.';
	  Tx[8]=(int)(temp_data)%10+'0';
	
	  LCD_printchar(0x3,1,Tx[5]);
		LCD_printchar(0x4,1,Tx[6]);
		LCD_printchar(0x5,1,Tx[7]);
		LCD_printchar(0x6,1,Tx[8]);
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
		GPIO_Configuration();
	  DS18B20_Init();
    /* 配置USART1 */
    USART1_Config();
		// ADC 初始化
	  ADCx_Init();  
    /* 初始化LCD1602 */
		LCD_init(); 
	
     
	LCD_printstring(0,0,"EC:"); 
  LCD_printstring(0x08,0,"mS/cm");	
	LCD_printstring(0,1,"T :"); 
  LCD_printstring(0x07,0,"C");		
	
  while(1)
	{	
		TEMP_Value_Conversion();
		EC_Value_Conversion();	
		
		printf("%s",Tx);
		delay_ms(1000);
		
	}	
}
/*********************************************END OF FILE**********************/
