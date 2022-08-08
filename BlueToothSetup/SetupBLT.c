// main.c
// Runs on LM4F120/TM4C123
// this connection occurs in the USB debugging cable
// U1Rx (PB0)
// U1Tx (PB1)
// Ground connected ground in the USB cable


// Header files 
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "string.h"

#define RED 		0x02
#define GREEN  	0x08
#define BLUE 		0x04
#define PURPLE 	0x06
#define WHITE 	0x0E
#define DARK 		0x00

void Delay(void);
void PortF_Init(void);

char Color;


// main function for programming BT device with no UI
//int mainNoUI(void) {
int main(void) {

	UART_Init();
	PortF_Init();

char String[30];
  // setup the HC-05 bluetooth module
		UART0_OutString("AT+NAME=Master\r\n"); 		// Name = Master
		UART1_OutString("AT+NAME=Master\r\n"); 		// Name = Master
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);
		
		Delay();	
			
		UART0_OutString("AT+UART=57600,0,1,\r\n"); // baud rate = 57600, 1 stop bit, odd parity
		UART1_OutString("AT+UART=57600,0,1,\r\n"); // baud rate = 57600, 1 stop bit, odd parity
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);
		
		Delay();		
			
		UART0_OutString("AT+PSWD=1111\r\n"); 		  // Pass = 1111
		UART1_OutString("AT+PSWD=1111\r\n"); 		  // Pass = 1111
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);
    
		Delay();		
			
		UART0_OutString("AT+ROLE=0\r\n");				  // Mode = Slave
		UART1_OutString("AT+ROLE=0\r\n");				  // Mode = Slave
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);

		Delay();		
			
  // query the HC-05 bluetooth module
		UART0_OutString("AT+UART?\r\n"); // baud rate = 57600, 1 stop bit, odd parity
		UART1_OutString("AT+UART?\r\n"); // baud rate = 57600, 1 stop bit, odd parity
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);
		
		Delay();		
			
		UART0_OutString("AT+PSWD?\r\n"); 		  // Pass = 1111
		UART1_OutString("AT+PSWD?\r\n"); 		  // Pass = 1111
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);
		

		UART0_OutString("AT+ROLE?\r\n");				  // Mode = Slave
		UART1_OutString("AT+ROLE?\r\n");				  // Mode = Slave
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
	
			
    UART0_OutString(String);
		UART0_OutString("AT+NAME?\r\n"); 		// Name = Master
		UART1_OutString("AT+NAME?\r\n"); 		// Name = Master
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);
			
			Delay();
			
		UART0_OutString(String);
		UART0_OutString("AT+VERSION?\r\n"); 		// Name = Master
		UART1_OutString("AT+VERSION?\r\n"); 		// Name = Master
    while ((UART1_FR_R&UART_FR_BUSY) != 0){};
    BLT_InString(String);
    UART0_OutString(String);

//  while (1) {

//		Color = UART1_InChar();
//		switch (Color){
//			case 'r' : GPIO_PORTF_DATA_R = RED;
//				break;
//			case 'b' : GPIO_PORTF_DATA_R = BLUE;
//				break;
//			case 'g' : GPIO_PORTF_DATA_R = GREEN;
//				 break;
//			case 'p' : GPIO_PORTF_DATA_R = PURPLE;
//				break;
//			case 'w' : GPIO_PORTF_DATA_R = WHITE;
//				break;
//			case 'd' : GPIO_PORTF_DATA_R = DARK;
//				break;
//			default : GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R;
//		}
//	}
}

void PortF_Init(){
	SYSCTL_RCGC2_R |= 0x20; 
	// Port F_Buttons
		
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   	// unlock PortF PF0  
	GPIO_PORTF_CR_R |= 0x1F;         		// allow changes to PF4-0 :11111->0x1F     
  GPIO_PORTF_AMSEL_R &= ~0x1F;        // disable analog function
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; 	// GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R &= ~0x11;          // PF4,PF0 input   
  GPIO_PORTF_DIR_R |= 0x0E;          	// PF3,PF2,PF1 output   
	GPIO_PORTF_AFSEL_R &= ~0x1F;        // no alternate function
  GPIO_PORTF_PUR_R |= 0x11;          	// enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R |= 0x1F;          	// enable digital pins PF4-PF0  

	GPIO_PORTF_IS_R 			&= ~0x11;       // PF4,0 is edge-sensitive
	GPIO_PORTF_IBE_R 			&= ~0x11;       // PF4,0 is not both edges
	GPIO_PORTF_IEV_R 			&= ~0x11;    		// PF4,0 falling edge event
	GPIO_PORTF_ICR_R 			|= 0x11;      	// clear flag4,0
	GPIO_PORTF_IM_R 			|= 0x11;      	// arm interrupt on PF4,0
	NVIC_PRI7_R 					|= (NVIC_PRI7_R&0xFF1FFFFF)|0x0020000; //priority 1
	NVIC_EN0_R 						|= 0x40000000;      // enable interrupt 30 in NVIC	
}


void Delay(){
	unsigned long volatile time;
  time = 2000000;  // 0.1sec
  while(time){
		time--;
  }
}
