#include <STC15.H>
#include <intrins.h>

typedef unsigned char BYTE;
sbit MOS_CON = P3^6;
sbit MCU_P3_7 = P3^7;


BYTE Inter_Frequency_CNT_Running = 0;//实际定时器中使用的计数值
/************** 延时函数声明 ***************/
void Delay500ms(void);


void PCA_isr() interrupt 7 using 1
{
	   //ADC_channel_start(ADC_CHANNEL_0);
	   if (CCF1)
	   {
			CCF1 = 0;
		
			CR = 0; //停止PCA定时器
			//  CR = 0; //停止PCA定时器
			// MCU_P3_7 = !MCU_P3_7;
			// LM3478_SW = !LM3478_SW;
			 		
		  MOS_CON = 1;
			MCU_P3_7 = 0;
			CL = CH = 0;//将PCA计数器清零
			 //CR = 1;
	   }
		 



	   
//		 if(CCF2)
//		 {
//				CCF2 = 0; //清中断标志
//			  //CR = 0; //停止PCA定时器
//			  MCU_P3_7 = 1;
//			  //CR = 1;
//		 
//		 }



}

void PCA_Init(void)
{
	P3M1 &= ~0xC0;
  P3M0 |=  0xC0;
	CCON = 0; //初始化PCA控制寄存器
	// 1. 先切换到第二组引脚（：CCP_S1=0, CCP_S0=1）
	P_SW1 &= ~(1<<5);  // CCP_S1 = 0
	P_SW1 |=  (1<<4);  // CCP_S0 = 1

	// 2. PCA基础初始化
	CMOD = 0x00; // PCA时钟 = 系统时钟/12(11.0592Mhz/12)
	CR = 0;// 先停止PCA计数
	CL = CH = 0;//将PCA计数器清零

	CCAPM0 = 0x00;  // 关闭PCA0所有功能，P3.5变回普通IO
//	CCAPM2 = 0x00;  // 关闭PCA0所有功能，P3.7变回普通IO
	// PCA1(P3.6)、PCA2(P3.7)：开启16位硬件PWM


	CCAPM1 = 0x4D;  // PCA1 → P3.6 高速脉冲翻转模式，并打开对应中断，因为最后需要关掉200us的PWM输出
	CCAPM2 = 0x4C;  // PCA2 → P3.7 高速脉冲翻转模式，但不打开中断，因为P3.7只是单纯的PWM输出，不需要中断服务

	CCAP1L = 0xB9;  // 初始占空比0%（全高）
	CCAP1H = 0x00;

	CCAP2L = 0x13;  // 初始占空比0%（全高）
	CCAP2H = 0x00;

	// 4. 使能
	//CR = 1; // 启动PCA计数器
	EA = 1;
	
}


void Timer0_Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0xDC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA = 1;					//使能全局中断
	
	
}



void Timer0_Isr(void) interrupt 1    //10ms进入一次
{
	  TL0 = 0x00;				//设置定时初始值
	  TH0 = 0xDC;				//设置定时初始值
	  TF0 = 0;				//清除TF0标志
	  	


		Inter_Frequency_CNT_Running++;//内触发模式下打开定时器自增
		if(Inter_Frequency_CNT_Running == 20)//到达设定的值1次，产生一次脉冲
		{
			 MOS_CON = 0;
			 MCU_P3_7 = 1;
			 CR = 1; //PCA定时器开始工作
			 Inter_Frequency_CNT_Running = 0;
		}
			
		
		


}

/************** 主函数 ***************/
void main()
{
		MCU_P3_7 = 1;
		MOS_CON = 1;
    
		PCA_Init();
	  Timer0_Init();
		
    while (1)
    {
        Delay500ms();

        // 如果后面要动态修改占空比，可直接在这里改
        // 例如:
        // CCAP1H = 200;
        // CCAP1L = 200;
        //
        // CCAP2H = 100;
        // CCAP2L = 100;
    }
}








/************** 延时函数 ***************/
void Delay500ms(void)
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 22;
    j = 3;
    k = 227;

    do
    {
        do
        {
            while (--k);
        } while (--j);
    } while (--i);
}
