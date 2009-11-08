#ifndef POWERBAR_CONFIG_INCLUDED
#define POWERBAR_CONFIG_INCLUDED

#define F_CPU 8000000UL // 8MHz

enum {
    NUM_RELAYS = 5 
};

#include <avr/io.h>

#define RELAY_PORT PORTD
#define RELAY_DDR  DDRD

#define LED_PORT PORTA
#define LED_DDR  DDRA
#define LED_PIN  PINA

#define TOGGLE_PORT PORTC
#define TOGGLE_PIN  PINC
#define TOGGLE_DDR  DDRC

#define ALL_BITS 0xf8

//==============================================================================

#define COM_RESET_PORT PORTB
#define COM_RESET_DDR DDRB
#define COM_RESET PB1

#define STATUS_LED_PORT PORTB
#define STATUS_LED_DDR DDRB
#define STATUS_LED PB3

#define DEBUG_PORT PORTC
#define DEBUG_DDR  DDRC
#define DEBUG_BIT  PC1

#define DTR_PORT PORTB
#define DTR_DDR  DDRB
#define DTR      PB0

#endif // POWERBAR_CONFIG_INCLUDED
