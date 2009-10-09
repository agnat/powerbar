
#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "user_interface.hpp"
#include "relay_controller.hpp"
#include "command_handler.hpp"

powerbar::relay_controller relay_ctl;
powerbar::user_interface   ui;
powerbar::command_handler  cmd_handler;
powerbar::command_handler::buffer_type & input_buffer = cmd_handler.input_buffer();

ISR(TIMER0_COMP_vect) {
    relay_ctl.tick();
    ui.tick();
}


ISR(USART_RXC_vect) {
    char c = UDR;
    if (bit_is_clear(UCSRA, FE)) {
        input_buffer.push_back(c);
    }
}

void
uart_9600_8n1(void) {
#define BAUD 9600
#include <util/setbaud.h>
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
#if USE_2X
    UCSRA |= (1 << U2X);
#else
    UCSRA &= ~(1 << U2X);
#endif

    UCSRC = _BV(URSEL) |  _BV(UCSZ1) | _BV(UCSZ0);
    UCSRB |= _BV(TXEN) | _BV(RXEN) | _BV(RXCIE);
}

inline
void
init() {
    // 1kHz Timer
    OCR0 = 125; // 1ms 
    TCCR0 |= _BV(WGM01) | _BV(CS01) | _BV(CS00); // CTC, prescale 64
    TIMSK |= _BV(OCIE0);

    COM_RESET_DDR |= _BV( COM_RESET );
    COM_RESET_PORT |= _BV( COM_RESET ); // release com device reset line

    DEBUG_DDR |= _BV(DEBUG_PIN);

    STATUS_LED_DDR |= _BV( STATUS_LED );

    uart_9600_8n1();

    sei();
}

int main() {
    init();
    while ((volatile bool)true) {
        ui.update_led_state( relay_ctl.current_state() );

        uint8_t toggle_events = ui.poll_toggle_events();
        if (toggle_events) {
            relay_ctl.toggle( toggle_events );
            uart_putchar('t');
            uart_putchar('\n');
        }

        cmd_handler.process_input();
    };
    return 0;
}
