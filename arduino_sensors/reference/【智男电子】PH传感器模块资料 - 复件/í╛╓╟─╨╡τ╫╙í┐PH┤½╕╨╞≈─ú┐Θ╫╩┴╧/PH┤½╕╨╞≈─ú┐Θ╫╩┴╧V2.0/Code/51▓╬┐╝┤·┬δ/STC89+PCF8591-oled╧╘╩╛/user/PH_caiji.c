/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * PCF8591:VCC -> 5V; GND -> GND; SCL -> P1.6;SDA -> P1.7; 
 *         OUT1~OUT8 -> P3��
 * OLED :CS -> P2.4; RST -> P2.2; DC -> P2.3;SCL -> P2.0;SDIN -> P2.1;
 *         D0~D7 -> P0��
 * PHģ��: VCC -> 5V; GND -> GND; PO -> AIN0;
 *
 * ��������������PHֵҺ����ʾ��
             ���ڽ��ղ������õ�PHֵ��������9600����
 *           ��������λ����ʾPHֵ��
�Ա����̵�ַ��https://shop269770855.taobao.com
**********************************************************************************/

#include <reg52.h>
#include <include.h>

typedef unsigned char uchar;
typedef unsigned char uint;

/**************�ⲿ��������************************/
int num=0; 
/**************��������***************************/
double PH=0.0;
unsigned char disbuff[5]={0};
double ADC_Voltage=0.0;
/*
 * ��������delay_1s
 * ����  ����ʱ����1s
 *         
 * ����  ��unsigned int time����ʱ��
 * ���  ����
 * ˵��  ��Һ����ʾ����ʱ����
 */
void delay_1s()	  //��ʱ1s
{
	unsigned char i, j, k;

	_nop_();
	i = 8;
	j = 1;
	k = 243;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}	

/*
 * ��������Get_Voltage_Value
 * ����  ����ȡADC0809��ת����ѹֵ
 *         
 * ����  ����
 * ���  ����
 * ˵��  ��
 *						AIN0(0X40):��������
 *						AIN1(0X41):����
 *					  AIN2(0X42):����������
 *					  AIN3(0X43):��������
 */
void Get_Voltage_Value()
{
			ISendByte(PCF8591,0x40);
      num=IRcvByte(PCF8591);  //ADC0 ģ��ת��1      PH�缫
		  ADC_Voltage =num * 5.0/ 255;  //ת��Ϊ��ѹֵ
}
/*
 * ��������Character_Conversion
 * ����  ����ʱ����
 *         
 * ����  ��unsigned char n����ʱ��
 * ���  ����
 * ˵��  ��
 */
void PH_Value_Conversion()
{	
	PH=-5.887*ADC_Voltage+21.677;
	
	if(PH<=0){PH=0;}
	if(PH>=14){PH=14;}
		
   /*��ʾ����*/
	disbuff[0]=(int)(PH*100)/1000+'0';
	disbuff[1]=(int)(PH*100)%1000/100+'0';	
	disbuff[2]='.';
	disbuff[3]=(int)(PH*100)%100/10+'0';
	disbuff[4]=(int)(PH*100)%10+'0';
	
}
void display_value()
{
	
  /*��ʾ PH Ũ��ֵ*/
	OLED_ShowChar(24,2,disbuff[0]);//��ʾASCII�ַ�
	OLED_ShowChar(33,2,disbuff[1]);//��ʾASCII�ַ�
	OLED_ShowChar(42,2,disbuff[2]);//��ʾASCII�ַ�
	OLED_ShowChar(51,2,disbuff[3]);//��ʾASCII�ַ�	
	OLED_ShowChar(60,2,disbuff[4]);//��ʾASCII�ַ�	
}
void StartUART( void )
{  							//������9600
     SCON = 0x50;
     TMOD = 0x20;
     TH1 = 0xFD;
     TL1 = 0xFD; 
     PCON = 0x00;
     TR1 = 1;
}
//**********ͨ�����ڽ����յ����ݷ��͸�PC��**************
void R_S_Byte(uchar R_Byte)
{	
	 SBUF = R_Byte;  
     while( TI == 0 );				//��ѯ��
  	 TI = 0;    
}
/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
void main(void)
{
	int i;
	StartUART();
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear()  	; 

	
	OLED_ShowCHinese(0,0,0);
	OLED_ShowCHinese(18,0,1);
	OLED_ShowCHinese(36,0,2);
	OLED_ShowCHinese(54,0,3);
	OLED_ShowCHinese(72,0,4);
	OLED_ShowCHinese(90,0,5);
	OLED_ShowCHinese(108,0,6);
	OLED_ShowString(0,2,"PH:");	
	
	while(1)
	{		
				Get_Voltage_Value();
				PH_Value_Conversion();
				display_value(); 
			for(i=0;i<5;i++)
				{
					R_S_Byte(disbuff[i]);
				}	
//				R_S_Byte('\r');//���ͻس����з�
//				R_S_Byte('\n');
				delay_1s();
  }	
}
