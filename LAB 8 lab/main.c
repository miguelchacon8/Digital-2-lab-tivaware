//LAB 8
//NOMBRE: MIGUEL CHACÓN
//CARNET: 21543
//CURSO: ELECTRONICA DIGITAL 2

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "driverlib/pin_map.h"

#define XTAL 40000000

#define DEBOUNCE_DELAY 5000 // 5000 cycles delay for debounce

uint32_t ui32Period;
char Dato;
char UltDat;

void InitUART(void);
void comunicacionLED(void);

void delay(uint32_t count)
{
    volatile uint32_t i;
    for (i = 0; i < count; i++);
}


int main(void){

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //configurando 40MHz



   //HABILITANDO EL PUERTO F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // TIMER
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //SE HABILITA EL LED RGB, PF1 = R, PF2 = B, PF3=G
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // Configure PF4 as input with pull-up resistor
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //TIMER como temporizador periódico de 32 bits
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (SysCtlClockGet())/2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
    //Habilitamos la interrupcion del timer A
    IntEnable(INT_TIMER0A);
    //INTERRUPCION POR TIMEOUT
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //INTERRUPCIONES GLOBALES
    IntMasterEnable();

    //INICIALIZACION DEL UART
    InitUART();

    //HABILITO EL TIMER
    TimerEnable(TIMER0_BASE, TIMER_A);


    //UART
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    //USB
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    //PWM
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    //ADC
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);


    while(1)
    {
//        // Wait for the button to be released
//        while ((GPIO_PORTF_DATA_R & (1 << 4)) == 0);
//
//        // Wait for a debounce delay
//        delay(DEBOUNCE_DELAY);
//        // Check if switch 1 is pressed
//        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0)
//        {
//            // VERDE
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
//            SysCtlDelay(20000000);
//            //PARPADEO
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
//            SysCtlDelay(10000000);
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
//            SysCtlDelay(20000000);
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
//            SysCtlDelay(10000000);
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
//            SysCtlDelay(20000000);
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
//            //SysCtlDelay(20000000);
//
//            //AMARILLO
//
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3|GPIO_PIN_1);
//            SysCtlDelay(20000000);
//
//            //ROJO
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
//            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1);
//            SysCtlDelay(80000000);
//
//            while ((GPIO_PORTF_DATA_R & (1 << 4)) == 0);
//
//            // Wait for a debounce delay
//            delay(DEBOUNCE_DELAY);
//        }
  }
}

//FUNCION DEL TIMER0
void Timer0IntHandler(void){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    comunicacionLED();

}
//************************************************************

////FUNCION DEL UART
void InitUART(void){

    //ENABLE PORTA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //ENABLE UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //PINS PERIPHERAL CONTROLLED
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);

    //CONFIG DEL UART
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART0);
   //HABILITO LAS INTERRUPCIONES DEL UART
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}
//************************************************************

////FUNCION DE la INTERRUPCION DEL UART
void UARTI0ntHandler(void){
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);

    while(UARTCharsAvail(UART0_BASE)) {
        Dato = UARTCharGetNonBlocking(UART0_BASE);
        // Revisa si el dato recibido es igual al anterior
        UARTCharPutNonBlocking(UART0_BASE, Dato);
        if(Dato != UltDat){
            UltDat = Dato;  // Si no es igual es porque quiere hacer  un toggle
        }
    }
}

void comunicacionLED(void){
    if(Dato == 'r'){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
    }
    else if(Dato == 'g'){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
            GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
    }
    else if(Dato == 'b'){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
    }
    else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
}

