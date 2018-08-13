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
//unsigned char duty = 30;
unsigned char received;
__bit update;
void Init(void) {
    //Configuration of pins. All are inputs except for TX(RC6) and PWM1(RC2) pins.
    TRISA = 0xff;
    TRISB = 0xff;
    TRISC = 0b10111011;
    
    /*      Baud rate generator configuration
     * 
     * Desired Baud Rate: As fast as max232 can handle (exceeding recommended maximal value of 120kbps)
     * SPBRG is the value at which the timer overflows. It can be calculated using this formula: SPBRG=[Fosc/(16*DESIRED_BAUD)-1]
     * 
     * SPBRG=[18 000 000/(16*230400)-1] = 4
     * 
     * To calculate the actual Baud Rate use this formula: BAUD_RATE = [Fosc/(16(SPBRG+1))]
     * 
     * BAUD_RATE = [18 000 000/(16(4+1))] =~ 225 000 
     * 
     * Lastly is important to calculate the error. It is done using this formula and using its absolute value: Error =|100 * (1 - DESIRED_BAUD / BAUD_RATE)|
     * 
     * Error = |100 * (1 - 230400/225000)| =  2.4%
     * 
     * It is emphasized to keep the error as low as possible. 
     */
    SPBRG = 4;
    
    //TXSTA: TRANSMIT STATUS AND CONTROL REGISTER (configuration)
    TXSTAbits.BRGH = 1;         //High speed baud rate: Baud Rate = FOSC /(16(X+1))
    TXSTAbits.SYNC = 0;         //Asynchronous mode of serial receiver and transmitter
    TXSTAbits.TXEN = 1;         //Transmit enable
    
    //RCSTA: RECEIVE STATUS AND CONTROL REGISTER (configuration)
    RCSTAbits.CREN = 1;         //Enables continuous reception
    RCSTAbits.SPEN = 1;         //Serial port enabled
    
    //PWM config:
    CCP1CON = 0b1100;           //Configures the CCP module to be PWM
    T2CON = 0b100;              //Timer2 is ON, with neither pre- nor postscaler
    PR2 = 100;                  //Setting the period
    CCPR1L = 0;
    
    //Configuring interrupts
    RCIE = 1;                   //Serial reception interrupt enable bit
    TXIE = 0;                   //Serial transmission interrupt enable bit
    PEIE = 1;                   //Peripheral interrupt enable bit
    GIE = 1;                    //Global interrupt enable bit
}

void __interrupt () tx (void) {
    if (RCIF) {
            RCIF = 0;
            received = RCREG;  
            CCPR1L = received;
            TXIE = 1;
            update = 1;
        }
    if (update){
        if (TXIF) {
            TXREG = received;
            update = 0;
            TXIE = 0;
        }       
    }
}


void main (void) {
    Init();
    while (1){
    }
}
