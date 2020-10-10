/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * PCF8591:VCC -> 5V; GND -> GND; SCL -> P1.6;SDA -> P1.7; 
 *         OUT1~OUT8 -> P3��
 * LCD1602:RS -> P2.5; RW -> P2.6; E -> P2.7;
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
  DisplaySingleChar(0x3,0,disbuff[0]);
	DisplaySingleChar(0x4,0,disbuff[1]);
	DisplaySingleChar(0x5,0,disbuff[2]);
	DisplaySingleChar(0x6,0,disbuff[3]);
	DisplaySingleChar(0x7,0,disbuff[4]);
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
	LCDInit();
	StartUART();
	
  DisplayString(0x0,0,"  Welcome  PH  ");
	DisplayString(0x0,1,"Detection system");
	delay_1s();
	WriteCmd(LCD_CLS);
	
	DisplayString(0x0,0,"PH=");
	
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
