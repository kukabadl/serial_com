/*
 * File:   Serial.c
 * Author: kuba
 *
 * Created on August 10, 2018, 9:50 PM
 */


#define _XTAL_FREQ 18000000

#include <xc.h>
#include <pic16f76.h>

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bit (Code protection off)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)

char counter = 0;

void Init(void) {
    //Configuration of pins. All are inputs except for the TX pin.
    TRISA = 0xff;
    TRISB = 0xff;
    TRISC = 0b10111111;
    
    /*      Baud rate generator configuration
     * 
     * Desired Baud Rate: 9600
     * SPBRG is the value at which the timer overflows. It can be calculated using this formula: SPBRG=[Fosc/(64*DESIRED_BAUD)-1]
     * 
     * SPBRG=[18 000 000/(64*9600)-1] = 28
     * 
     * To calculate the actual Baud Rate use this formula: BAUD_RATE = [Fosc/(64(SPBRG+1))]
     * 
     * BAUD_RATE = [18 000 000/(64(28+1))] =~ 9698 
     * 
     * Lastly is important to calculate the error. It is done using this formula and using its absolute value: Error =|100 * (1 - DESIRED_BAUD / BAUD_RATE)|
     * 
     * Error = |100 * (1 - 9600/9698)| =  1.01      
     * 
     * It is emphasized to keep the error as low as possible. 
     */
    SPBRG = 28;
    
    //TXSTA: TRANSMIT STATUS AND CONTROL REGISTER (configuration)
    TXSTAbits.BRGH = 0;         //Low speed baud rate: Baud Rate = FOSC /(64(X+1))
    TXSTAbits.SYNC = 0;         //Asynchronous mode of serial receiver and transmitter
    TXSTAbits.TXEN = 1;         //Transmit enable
    
    //RCSTA: RECEIVE STATUS AND CONTROL REGISTER (configuration)
    RCSTAbits.CREN = 1;         //Enables continuous reception
    RCSTAbits.SPEN = 1;         //Serial port enabled
    
    //Configuring interupts
    TXIE = 1;                   //Serial transmission interrupt enable bit
    PEIE = 1;                   //Peripheral interrupt enable bit
    GIE = 1;                    //Global interrupt enable bit
}

void __interrupt () tx (void) {
    if (TXIF){
        TXIF = 0;
        TXREG = counter;
        counter++;
        //__delay_ms(1);
    }
}


void main (void) {
    Init();
    while (1){
    }
}
