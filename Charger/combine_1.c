#include "ML51.H" 
#include <string.h>
#define Sint8  signed   char
#define Uint8  unsigned char
#define Uint16 unsigned int
#define Uint32 unsigned long
unsigned int xdata ADCdataAIN;
         int VREF=5;       
/*********************************************************************
** Define address
*********************************************************************/
//TC72
#define Adr_R               0x00
#define Adr_W               0x80
#define Adr_TMSB            0x02
#define Adr_TLSB            0x01
#define Shotdown            0x05
#define OneShot             0x15
/*********************************************************************
**  I/O Declaration
*********************************************************************/
/*
//thermal
#define PWM     P15     
#define SCK     P31     
#define SCS1    P24     
#define SDIO    P30     
#define SCS2		P20 		
#define SDI			P25
#define SCS3		P23
//power allocate
#define ADC			P21
#define TURN		P14
#define EN			P22
//LCD
#define SDIN		P00
#define SCLK		P01
#define DC			P02
#define RES			P03
#define CS			P52
*/

#define SDIN		P00  //LCD
#define SCLK		P01  //LCD
#define DC			P02  //LCD
#define RES			P03  //LCD

#define TURN		P14   //power
#define PWM     P15   //thermal

#define SCS2		P20 	//thermal
#define ADC			P21  //power allocate  I
#define EN			P22  //power allocate
#define SCS3		P23   //thermal
#define SCS1    P24   //thermal  
#define SDI			P25   //thermal

#define SDIO    P30   //thermal        I
#define SCK     P31   //thermal 

#define CS			P52  //LCD
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xBF
/*********************************************************************
**  LCD pattern display
*********************************************************************/
unsigned char code LCD_basic[][19]={
	0x00,0x00,0x7E,0x11,0x11,0x11,0x7E,0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00,0x36,0x36,0x00,0x00,
	0x00,0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00,0x36,0x36,0x00,0x00,
	0x00,0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00,0x36,0x36,0x00,0x00,
	0x00,0x60,0x58,0x46,0x43,0x46,0x58,0x60,0x01,0x01,0x7F,0x01,0x01,0x00,0x00,0x36,0x36,0x00,0x00,
};
unsigned char code Watt_basic[][6]={
	0x7E,0x11,0x11,0x11,0x7E,0x00,//A
	0x7C,0x04,0x18,0x04,0x7C,0x00,//m
	0x7C,0x14,0x14,0x14,0x08,0x00,//p
	0x00,0x36,0x36,0x00,0x00,0x00,//:
	0x7F,0x49,0x49,0x49,0x41,0x00,//E
	0x7F,0x08,0x08,0x08,0x7F,0x00,//H
	0x00,0x00,0x00,0x00,0x00,0x00,//space
	0x3F,0x40,0x38,0x40,0x3F,0x00,//W
	0x20,0x54,0x54,0x54,0x78,0x00,//a
	0x04,0x3F,0x44,0x40,0x20,0x00,//t
	0x04,0x3F,0x44,0x40,0x20,0x00,//t
	0x00,0x36,0x36,0x00,0x00,0x00,//:
	0x38,0x44,0x44,0x44,0x20,0X00,//c
	0x7F,0x08,0x04,0x04,0x78,0x00,//h
	0x20,0x54,0x54,0x54,0x78,0x00,//a
	0x7C,0x08,0x04,0x04,0x08,0x00,//r
	0x38,0x44,0x44,0x44,0xF8,0x00,//g
	0x38,0x54,0x54,0x54,0x18,0x00,//e
	0x38,0x44,0x44,0x48,0x7F,0x00,//d
	0x00,0x36,0x36,0x00,0x00,0x00,//:
	0x01,0x02,0x02,0x02,0x01,0x00,//g con
};
unsigned char code Watt_unit[][6]={
	0x7C,0x04,0x18,0x04,0x7C,0x00,//m
	0x7E,0x11,0x11,0x11,0x7E,0x00,//A
	0x7E,0x05,0x18,0x04,0x7C,0x00,//m+g
	0x7E,0x11,0x11,0x11,0x7E,0x00,//A
	0x7F,0x08,0x04,0x04,0x78,0x00,//h
	0x7C,0x04,0x18,0x04,0x7C,0x00,//m 
	0x3F,0x40,0x38,0x40,0x3F,0x00,//W
	0x01,0x02,0x02,0x02,0x01,0x00,
	0x07,0x05,0x07,0x00,0x3E,0x41,//oC
	0x41,0x41,0x22,0x00,0x00,0x00,
};
unsigned char code number[][5]={
	{0x3E,0x51,0x49,0x45,0x3E},		//   ( 16)  0 - 0x0030 Digit Zero
	{0x00,0x42,0x7F,0x40,0x00},		//   ( 17)  1 - 0x0031 Digit One
	{0x42,0x61,0x51,0x49,0x46},		//   ( 18)  2 - 0x0032 Digit Two
	{0x21,0x41,0x45,0x4B,0x31},		//   ( 19)  3 - 0x0033 Digit Three
	{0x18,0x14,0x12,0x7F,0x10},		//   ( 20)  4 - 0x0034 Digit Four
	{0x27,0x45,0x45,0x45,0x39},		//   ( 21)  5 - 0x0035 Digit Five
	{0x3C,0x4A,0x49,0x49,0x30},		//   ( 22)  6 - 0x0036 Digit Six
	{0x01,0x71,0x09,0x05,0x03},		//   ( 23)  7 - 0x0037 Digit Seven
	{0x36,0x49,0x49,0x49,0x36},		//   ( 24)  8 - 0x0038 Digit Eight
	{0x06,0x49,0x49,0x29,0x1E},		//   ( 25)  9 - 0x0039 Dight Nine
	{0x00,0x60,0x60,0x00,0x00},   //   ( 14)  . - 0x002E Full Stop
	{0x00,0x00,0x00,0x00,0x00},
};
/*********************************************************************
**  Variable Declaration
*********************************************************************/
Uint16 i;//for first  loop counting
Uint16 j;//for second loop counting

/*********************************************************************
**  LCD display
*********************************************************************/
//  Delay Time
void uDelay(unsigned char l)
{
	while(l--);
}


void Delay(unsigned char n)
{
unsigned char i,j,k;

	for(k=0;k<n;k++)
	{
		for(i=0;i<131;i++)
		{
			for(j=0;j<15;j++)
			{
				uDelay(203);	
			}
		}
	}
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Read/Write Sequence
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Write_Command(unsigned char Data)
{
	P5&=0xFB;//CS(P52)=0;
	DC=0;
	for (i=0; i<8; i++)
	{
		SCLK=0;//P1 & 0x80
		SDIN=(Data&0x80)>>7;
		Data = Data << 1;
		_nop_();
		SCLK=1;
		_nop_();
	}
	SCLK=0;
	DC=1;
	P5|=0x04;//CS(P52)=1;
}


void Write_Data(unsigned char Data)
{
	P5&=0xFB;//CS(P52)=0;
	DC=1;
	for (i=0; i<8; i++)
	{
		SCLK=0;
		SDIN=(Data&0x80)>>7;
		Data = Data << 1;
		_nop_();
		SCLK=1;
		_nop_();
	}
	SCLK=0;
	DC=1;
	P5|=0x04;//CS(P52)=1;
}

//  Show Regular Pattern (Full Screen)
void Fill_RAM(unsigned char Data)
{
unsigned char i,j;

	for(i=0;i<8;i++)
	{
		//Instruction_Setting('I',i,0x00);//Set_Start_Page(i);
		Write_Command(0xB0|i);
		//Instruction_Setting('A',0x00,0x00);//Set_Start_Column(0x00);
		Write_Command(0x00);
		Write_Command(0x10);
		
		for(j=0;j<128;j++)
		{
			Write_Data(Data);
		}
	}
}
//Initialize
void OLED_Init(){
unsigned char i;

	RES=0;
	for(i=0;i<200;i++)
	{
		uDelay(200);
	}
	RES=1;

	 Write_Command(0xae); /* set  display off */   
   Write_Command(0x00);    
   Write_Command(0x10);
   Write_Command(0xD5); /* set  lower column start address */
   Write_Command(0xA0); /* set  higher column start address */  
   Write_Command(0xA8); /* set  display start line */   
   Write_Command(0x3F);   
   Write_Command(0xD3); /* set  contrast control */
   Write_Command(0x00);    
   Write_Command(0x40);    
   Write_Command(0x00);   
   Write_Command(0x20); /* set  segment remap  */  
   Write_Command(0x02); /* set  normal display */   
   Write_Command(0xa1); /* set  multiplex ratio */
   Write_Command(0xC8); /* 1/64 */   
   Write_Command(0xDA); /* master configuration */
   Write_Command(0x12); /* external vcc supply */  
   Write_Command(0xc8); /* set  com scan direction */  
   Write_Command(0xd3); /* set  display offset  */
   Write_Command(0x40);   
   Write_Command(0x81); /* set  display clock divide/oscillator frequency */
   Write_Command(0xBF);    
   Write_Command(0xD9);    /*set area color mode off */
   Write_Command(0x25);       
   Write_Command(0xA4);
   Write_Command(0xA6);    
   Write_Command(0xaf); /* set  display on */
}



void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d){
unsigned char i,j;

	for(i=a;i<(b+1);i++)
	{
		Write_Command(0xB0|i);
		
		Write_Command(0x00+c%16);
		Write_Command(0x10+c/16);
		
		for(j=0;j<d;j++)
		{
			Write_Data(*Data_Pointer);
			Data_Pointer++;
		}
	}
}

void set_LCD_display(void){
	unsigned char i;
	//FADE_IN
	Write_Command(0xAF);//Set_Display_On
	for(i=0;i<(0xBF+1);i++)
	{
		Write_Command(0x81);				
		Write_Command(i);
		uDelay(200);
		uDelay(200);
		uDelay(200);
	}
	//fill_ram
	for(i=0;i<8;i++)
	{
		//Instruction_Setting('I',i,0x00);//Set_Start_Page(i);
		Write_Command(0xB0|i);
		//Instruction_Setting('A',0x00,0x00);//Set_Start_Column(0x00);
		Write_Command(0x00);
		Write_Command(0x10);
		
		for(j=0;j<128;j++)
		{
			Write_Data(0xFF);
		}
	}
	Delay(5);
	//clear screen
	for(i=0;i<8;i++)
	{
		//Instruction_Setting('I',i,0x00);//Set_Start_Page(i);
		Write_Command(0xB0|i);
		//Instruction_Setting('A',0x00,0x00);//Set_Start_Column(0x00);
		Write_Command(0x00);
		Write_Command(0x10);
		
		for(j=0;j<128;j++)
		{
			Write_Data(0x00);
		}
	}
	Delay(5);
	//Frame
	Write_Command(0xB0|0x00);	
	Write_Command(0x00);
	Write_Command(0x10);
	for(i=0;i<Max_Column;i++)
	{
		Write_Data(0x01);
	}
	Write_Command(0xB0|0x07);	
	Write_Command(0x00);
	Write_Command(0x10);
	for(i=0;i<Max_Column;i++)
	{
		Write_Data(0x80);
	}
	for(i=0;i<8;i++)
	{
		//Instruction_Setting('I',i,0x00);//Set_Start_Page(i);
		Write_Command(0xB0|i);		
		for(j=0;j<Max_Column;j+=(Max_Column-1))
		{
			//Instruction_Setting('A',XLevel+j,0x00);//Set_Start_Column(XLevel+j);
			Write_Command(0x00+(XLevel+j)%16);
			Write_Command(0x10+(XLevel+j)/16);
			Write_Data(0xFF);
		}
	}
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Start Page
//    b: End Page
//    c: Start Column
//    d: Total Columns
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void show_data(Uint16 D1,unsigned char RC){
	Uint16 buff;
	unsigned char j;

	Write_Command(0xB0|RC);//page

	Write_Command(0x00+(0x15)%16);
	Write_Command(0x10+(0x15)/16);
	buff=D1/100;//quotient
	if(buff!=0){
		for(j=0;j<5;j++)
		{
			Write_Data(number[buff][j]);	
		}
	}
	Write_Command(0x00+(0x1B)%16);
	Write_Command(0x10+(0x1B)/16);
	D1=D1-buff*100;
	buff=D1/10;//printf("buff=%d\n",buff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[buff][j]);	
	}
	Write_Command(0x00+(0x21)%16);
	Write_Command(0x10+(0x21)/16);
	buff=D1%10;//printf("buff=%d\n",buff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[buff][j]);	
	}
}

void show_mA(float f1){
	Uint16 ff;
	unsigned char j;

	Write_Command(0xB0|0x02);//page mA

	Write_Command(0x00+(0x55)%16);
	Write_Command(0x10+(0x55)/16);
	ff=f1/100;//quotient
	if(ff!=0){
		for(j=0;j<5;j++)
		{
			Write_Data(number[ff][j]);	
		}
	}
	Write_Data(0x00);
	f1=f1-ff*100;
	ff=f1/10;//printf("buff=%d\n",ff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[ff][j]);	
	}
	Write_Data(0x00);
	f1=f1-ff*10;
	ff=f1/1;//printf("buff=%d\n",ff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[ff][j]);	
	}
	Write_Data(0x00);
	Write_Data(0x60);//dot
	Write_Data(0x60);//dot
	Write_Data(0x00);
	f1=f1-ff;
	ff=f1/0.1;//printf("buff=%d\n",buff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[ff][j]);	
	}
}

void show_mW(float f1,unsigned char RC){
	Uint16 ff;
	unsigned char j;

	Write_Command(0xB0|RC);//page mA

	Write_Command(0x00+(0x3D)%16);
	Write_Command(0x10+(0x3D)/16);
	ff=f1/100;//quotient
	if(ff!=0){
		for(j=0;j<5;j++)
		{
			Write_Data(number[ff][j]);	
		}
	}
	Write_Data(0x00);
	f1=f1-ff*100;
	ff=f1/10;//printf("buff=%d\n",ff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[ff][j]);	
	}
	Write_Data(0x00);
	f1=f1-ff*10;
	ff=f1/1;//printf("buff=%d\n",ff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[ff][j]);	
	}
	Write_Data(0x00);
	Write_Data(0x60);//dot
	Write_Data(0x60);//dot
	Write_Data(0x00);
	f1=f1-ff;
	ff=f1/0.1;//printf("buff=%d\n",buff);
	for(j=0;j<5;j++)
	{
		Write_Data(number[ff][j]);	
	}
}

//  Fade Out (Full Screen)
void Fade_Out(){
unsigned int i;	

	for(i=(Brightness+1);i>0;i--)
	{
		//Instruction_Setting('D',i-1,0x00);//Set_Contrast_Control(i-1);
		Write_Command(0x81);				// Set Contrast Control for Bank 0
		Write_Command(i-1);
		uDelay(200);
		uDelay(200);
		uDelay(200);
	}
	Write_Command(0xAE);//Set_Display_Off
}

//  Sleep Mode
//    "0x00" Enter Sleep Mode
//    "0x01" Exit Sleep Mode
void Sleep(unsigned char a){
	switch(a)
	{
		case 0://sleep
			Write_Command(0xAE);//Set_Display_On_Off(0xAE);
			Write_Command(0xA5);//Set_Entire_Display(0xA5);
			break;
		case 1://wake
			Write_Command(0xA4);//Set_Entire_Display(0xA4);
			Write_Command(0xAF);//Set_Display_On_Off(0xAF);
			break;
	}
}


//TC72 SPI
void ByteWrite(Uint8 src){
	//Uint8 reg;
		SCK=0;
		//reg=0x00;
		for (i = 0; i < 8; i++)
		{
			SCK = 1;
			src = src << 1;
			SDI =(src & 0x80)>>7;							
			_nop_();
			SCK = 0;
			_nop_();
		}
}
Uint8 ByteRead(void){
    //Uint8 i;
		Uint8 tmp;

    for (i = 0; i < 8; i++)
    {
				SCK = 1;
        _nop_();
        SCK = 0;
				if (SDIO)
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
				_nop_();
    }

    return tmp;
}









void  	InitThermal(void);
void   	InitLCD(void);
void   	InitPin(void);
float  	I_detecting(void);
Uint16 	Thermal_detecting(unsigned char select);
Uint16 	MaxRead(void);
void   	pwm_open(unsigned long u32SYSCLK, unsigned int u16CNT, unsigned int u16DLYUnit);
void    pwm_modulate(Uint16 AT,Uint16 TH,Uint16 TC);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Main Program
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
unsigned int change, cycle, u16CNT;
void main()
{
  Uint8 r=0;
	Uint16 T1,T2,T3;
	float Ampere;
	
	/******Setup******/
	//ALL Initialize
	InitPin();
	//UART
	MFP_P16_UART0_TXD;
  P16_QUASI_MODE;
  UART_Open(24000000,UART0_Timer3,115200);
  ENABLE_UART0_PRINTF;
	//SCS
	P2&=0xF7;//SCS3=0,P23
	P2|=0x11;//SCS1=1,SCS2=1;P24,P20
	//PWM
	change=0;
	cycle=2000;
	//Time
	u16CNT=12000;//120*100*10000(2min=2*60*100*10ms)
	
	//reset
	T1=0;
	T2=0;
	T3=24;
	Ampere=0;
	
	TURN=1;
	OLED_Init();
	Delay(1);
	EN=1;
	
	/*******execute*********/
	while(1)
	{
		set_LCD_display();
		
		Show_Pattern(&LCD_basic[0][0],0x02,0x05,XLevelL+0x03,0x13);
		Show_Pattern(&Watt_basic[0][0],0x02,0x02,XLevelL+0x3D,0x18);
		Show_Pattern(&Watt_basic[4][0],0x03,0x03,XLevelL+0x3D,0x30);
		Show_Pattern(&Watt_basic[12][0],0x05,0x05,XLevelL+0x3D,0x30);
		Show_Pattern(&Watt_basic[20][0],0x06,0x06,XLevelL+0x5B,0x06);
		Show_Pattern(&Watt_unit[0][0],0x02,0x02,XLevelL+0x6F,0x0C);
		Show_Pattern(&Watt_unit[2][0],0x06,0x06,XLevelL+0x58,0x12);
		Show_Pattern(&Watt_unit[5][0],0x04,0x04,XLevelL+0x58,0x0C);
		Show_Pattern(&Watt_unit[7][0],0x06,0x06,XLevelL+0x55,0x03);
		for(r=2;r<6;r++){
			Show_Pattern(&Watt_unit[8][0],r,r,XLevelL+0x27,0x09);
		}
		/*
		for(r=0;r<10;r++){
			
			T1 = Thermal_detecting('1');
			printf("T1:%d\n",T1);
			T2 = Thermal_detecting('2');
			printf("T2:%d\n",T2);
			
			show_data(T1,0x03);
			show_data(T2,0x04);
			show_data(T3,0x02);
			show_data(T1-T2,0x05);
			
			
			show_mA(Ampere);
			show_mW(Ampere*5.1,0x04);
			show_mW(Ampere/5.1,0x06);
			
			Delay(10);	
		}
		*/
		Fill_RAM(0x00);			// Clear Screen
		Fade_Out();
		
	}
	
}

void InitThermal(){
		//PWM,SCK,SCS1,SDIO,
	  //SCS2,SCS3,SDI 
		MFP_P15_GPIO,MFP_P31_GPIO,MFP_P30_GPIO,MFP_P25_GPIO;
		//QUASI_MODE
		P15_QUASI_MODE,P31_PUSHPULL_MODE,P30_INPUT_MODE,P25_PUSHPULL_MODE;
		//P24
		SFRS = 1;
		P2M1 &= ~BIT4;
		P2M2 |=  BIT4;
	
		//P23
		SFRS = 1;
		P2M1 &= ~BIT3;
		P2M2 |=  BIT3;
	
		//P20
		SFRS = 1;
		P2M1 &= ~BIT0;
		P2M2 |=  BIT0;
		//,MFP_P20_GPIO,P20_PUSHPULL_MODE
		//SFRS = 1;
		//P2M1 &= ~BIT0;
		//P2M2 &= ~BIT0;
}
void InitLCD(){
		//SDIN,SCLK,DC,RES,CS QUASI mode
		MFP_P00_GPIO,MFP_P01_GPIO,MFP_P03_GPIO,MFP_P52_GPIO;
		P00_QUASI_MODE,P01_QUASI_MODE,P03_QUASI_MODE,P52_QUASI_MODE;
		//P02(DC) QUASI mode
		SFRS = 1;
		P0M1 &= ~BIT2;
		P0M2 &= ~BIT2;
}
void InitPin(){
		//ADC
		ENABLE_ADC_CH4;
		//single
		SFRS=0;
		ADCCON1&=0xEF; 
		//sampling time
		SFRS=0;
		ADCSR &= 0x8F;
		ADCSR |= 0x30;//ADCDIV=Fsys/8
		ADCCON2&=0xF1;
		ADCCON2|=(0x07)<<1;
		
		//TURN,EN
		MFP_P14_GPIO,MFP_P22_GPIO;
		P14_QUASI_MODE,P22_QUASI_MODE;
		//Thermal,LCD
		InitThermal();
		InitLCD();
}
Uint16 Thermal_detecting(unsigned char select){
		//select CS turn on
		Uint16 temp=0x00;
		switch(select){
			case '1':
				SCS1=0;//MAX6675 T1
				temp=MaxRead();
				printf("T1 data:%x\n");
				//return temp;
				break;
			case '2':
				SCS2=0;//MAX6675 T2
				temp=MaxRead();
				printf("T2 data:%x\n");
				//return temp;
				break;
			case '3':
				SCS3=1;//TC72
				//write mode//
				_nop_();
				ByteWrite(Adr_W);
				ByteWrite(OneShot);
				//read temp//
				_nop_();
				SCS3=0;
				_nop_();
			  SCS3=1;
				ByteWrite(Adr_TMSB);
				temp=ByteRead();
				//temp=temp<<8;
				//ByteWrite(Adr_TLSB);
				//temp=ByteRead();
				printf("Room data:0x%x\n",temp);
				//return temp;
				break;
			default:
				break;
		}
		//all turn off
		SCS3=0,SCS1=1,SCS2=1;
		return temp;
}

Uint16 MaxRead(void){
//  Uint16 i;
	 Uint16 tmp;
	 _nop_();
		//falling edge of clock 
    for (i = 0; i < 16; i++)
    {
		SCK = 1;
        if (SDIO)
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;
        
        SCK = 0;
    }
	tmp=tmp>>3;
	tmp=tmp/4;
  return tmp;
}

void pwm_modulate(Uint16 AT,Uint16 TH,Uint16 TC){
	if((TC-AT>20)&&((TH-TC)<10)){//(TC-AT>20)&&((TH-TC)<10)
		PWM=1;
		change=(TH-TC/50)*cycle;//reg=(TH-TC/50)
	}
	else{
		PWM=0;
	}
}

/*
	SCK=0;
	while(1){
		Delay(10);
		SCK=~SCK;
		Delay(10);
		SCK=~SCK;
	}
  SCK=0;
	while(1){
		for(r=0;r<10;r++){
			Delay(10);
		}
		SCK=~SCK;
	}
*/