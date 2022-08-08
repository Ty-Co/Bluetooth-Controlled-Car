// Documentation
// Project: Bluetooth Controlled Robot Car
// Description: Use bluetooth module and uart to communicate with robot car
// Name: Ty-Co
// YOUTUBE VIDEO: https://www.youtube.com/watch?v=ZrdDPIrbFUQ&ab_channel=b




#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "string.h"


#define SYSCTL_RIS_R            (*((volatile unsigned long *)0x400FE050))
#define SYSCTL_RIS_PLLLRIS      0x00000040  // PLL Lock Raw Interrupt Status
#define SYSCTL_RCC_R            (*((volatile unsigned long *)0x400FE060))
#define SYSCTL_RCC_XTAL_M       0x000007C0  // Crystal Value
#define SYSCTL_RCC_XTAL_6MHZ    0x000002C0  // 6 MHz Crystal
#define SYSCTL_RCC_XTAL_8MHZ    0x00000380  // 8 MHz Crystal
#define SYSCTL_RCC_XTAL_16MHZ   0x00000540  // 16 MHz Crystal
#define SYSCTL_RCC2_R           (*((volatile unsigned long *)0x400FE070))
#define SYSCTL_RCC2_USERCC2     0x80000000  // Use RCC2
#define SYSCTL_RCC2_DIV400      0x40000000  // Divide PLL as 400 MHz vs. 200
                                            // MHz
#define SYSCTL_RCC2_SYSDIV2_M   0x1F800000  // System Clock Divisor 2
#define SYSCTL_RCC2_SYSDIV2LSB  0x00400000  // Additional LSB for SYSDIV2
#define SYSCTL_RCC2_PWRDN2      0x00002000  // Power-Down PLL 2
#define SYSCTL_RCC2_BYPASS2     0x00000800  // PLL Bypass 2
#define SYSCTL_RCC2_OSCSRC2_M   0x00000070  // Oscillator Source 2
#define SYSCTL_RCC2_OSCSRC2_MO  0x00000000  // MOSC

#define SYSDIV2 7 // PLL 50MHz


#define DIRECTION       (*((volatile unsigned long *)0x400053FC)) //PORTB DATA REGISTER
#define LIGHT      			(*((volatile unsigned long *)0x400253FC)) //PORTF DATA REGISTER

#define PERIOD 	        25000          



#define F_B 0x0A //forward backward

#define RED 		0x02
#define GREEN  	0x08
#define YELLOW  0x0A
#define BLUE 		0x04
#define PURPLE 	0x06
#define WHITE 	0x0E
#define DARK 		0x00

extern void DisableInterrupts(void); //Disable interrupts
extern void EnableInterrupts(void);  //Enable interrupts
extern void WaitForInterrupt(void);  //low power mode

void PLL_Init(void);
void PWM0_0_Init(unsigned long dutyL, unsigned long dutyR);

void PortF_Init(void);
void PortB_Init(void);

void debounce(void);
unsigned long H = PERIOD/2;
unsigned long speed_count;
unsigned long dFlag; // direction flag
const unsigned long Delta = 2500;
unsigned short Speed = 0;
unsigned char Input;

int main(void){

	PLL_Init();
	UART_Init();	
	PortF_Init();
	PortB_Init();

	while(1){
	Input= UART1_InChar();// UART1_OutChar(Input);
		switch(Input){
			case 'w':
				GPIO_PORTF_DATA_R = GREEN;
				GPIO_PORTB_DATA_R &= ~0x0F;
				GPIO_PORTB_DATA_R |= F_B;
				PWM0_0_Init(H/2,H/2);
				break;
			case 's': 
				GPIO_PORTF_DATA_R = BLUE;
				GPIO_PORTB_DATA_R &= ~0x0F;
				GPIO_PORTB_DATA_R |= ~F_B;
				PWM0_0_Init(H/2,H/2);
				break;
			case 'a':
				GPIO_PORTF_DATA_R = YELLOW;
				PWM0_0_Init(H/3,H/4);
				break;
			case 'd':
				GPIO_PORTF_DATA_R = PURPLE;
				PWM0_0_Init(H/4,H/3);
				break;
			case 't':
				GPIO_PORTF_DATA_R = DARK;
				PWM0_0_Init(0,0);
				break;
			case 'l':
				if(H/2>Delta) H = 2*(H/2 - Delta);
				PWM0_0_Init(H/2,H/2);
				break;				
			case 'u':
				if(H/2<(PERIOD-Delta)) H = 2*(H/2 + Delta);
				PWM0_0_Init(H/2,H/2);
				break;
		}
		
	}

}




void PortF_Init(void){
	
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock portf for port 0
	GPIO_PORTF_CR_R |= 0x1F;
	GPIO_PORTF_AMSEL_R &= ~0x1E; // disable analog for PF4-0
	GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // GPIO clear PCTL bits for PF4-0
	GPIO_PORTF_DIR_R |= 0x0E; 
	GPIO_PORTF_DIR_R &= ~0x11; // 
	GPIO_PORTF_AFSEL_R &= ~0x1F;
	GPIO_PORTF_PUR_R |= 0x11;
	GPIO_PORTF_DEN_R |= 0x1F;
		
	//interrupt stuff
	GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11;    //     PF4 negative edge detect
	GPIO_PORTF_IEV_R &= ~0x11;    //     
	GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
	GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF1 AND PF4
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00400000; // (g) priority 2
	NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC   

}


void PortB_Init(void){
	
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000002;
	GPIO_PORTB_AMSEL_R &= ~0x0F; //disable analog functionality on PB0-3
	GPIO_PORTB_PCTL_R  &= ~0x0000FFFF;
	GPIO_PORTB_DIR_R   |= 0x0F; // 1 = ouput , PB0-3
	GPIO_PORTB_AFSEL_R &= ~0x0F; //disable alt funct on PB0-3
	GPIO_PORTB_DEN_R   |= 0x0F; // enable data pins 
}

void debounce(void){
	unsigned long i;
	for(i = 0; i < 60000; ++i);
		
}



void PWM0_0_Init(unsigned long dutyL, unsigned long dutyR){//PB7-6
	SYSCTL_RCGCPWM_R |= 0X00000001; //enable PWM module 0 clock source
	while((SYSCTL_PRGPIO_R&0x20)==0);
	GPIO_PORTB_AFSEL_R |= 0xC0; //enable PB7-6 as alternate function output
	GPIO_PORTB_DEN_R |= 0xC0;
	GPIO_PORTB_PCTL_R |= (GPIO_PORTB_PCTL_R&(~0xFF000000))|0x44000000;//enable M0PWM1-0 on pins PB7-6
	SYSCTL_RCC_R  = (SYSCTL_RCC_R&(~0x000E0000))|0x00100000; //clear bits 17-19 of PWMDIV for /2 divisor and enable USEPWMDIV (bit 20)
	PWM0_0_CTL_R  = 0; //count-down mode
	PWM0_0_GENB_R = 0xC08; //low on LOAD, high on CMPB down
	PWM0_0_GENA_R = 0xC8;  //low on LOAD, high on CMPA down
	PWM0_0_LOAD_R = PERIOD - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPB_R = dutyL - 1;             // 6) count value when output rises
	PWM0_0_CMPA_R = dutyR	- 1;             // 6) count value when output rises
	PWM0_0_CTL_R |= 0x00000001; //enable PWM module 0
	PWM0_ENABLE_R |= 0x00000003;          //enable PWM0/M0PWM1-0
}



void PLL_Init(void){
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;   //Step 0:  Allow usage of RCC2 for advanced clocking features
	SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;   //Step 1:  BYPASS PLL... clock divider not in use
	SYSCTL_RCC_R  &= ~SYSCTL_RCC_XTAL_M;    //Step 2:  clear XTAL field
  SYSCTL_RCC_R  += SYSCTL_RCC_XTAL_16MHZ; //         configure XTAL for 16 MHz crystal
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;//         clear oscillator source field
  SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;//         configure for main oscillator source
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;   //Step 3:  clear PWRDN2 bit to activate PLL
	SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;    //         use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~0x1FC00000) //Step 4: clear system clock divider field
                  + (SYSDIV2<<22);        //            configure for 50 MHz clock
	while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS)==0);//Step 5: wait for the PLL to lock by polling PLLLRIS
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;  //Step 6: Clear BYPASS
}

