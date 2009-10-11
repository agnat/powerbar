
#include "config.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "connection_handler.hpp"
#include "user_interface.hpp"
#include "relay_controller.hpp"
#include "model.hpp"
#include "save_timer.hpp"

powerbar::model model;
powerbar::relay_controller relay_ctl(model);
powerbar::user_interface   ui;
powerbar::connection_handler  cmd_handler(model, relay_ctl);
powerbar::save_timer saver_(model, relay_ctl);

ISR(TIMER0_COMP_vect) {
    relay_ctl.tick();
    ui.tick();
    cmd_handler.tick();
    saver_.tick();
}

ISR(INT2_vect) {
    cmd_handler.on_carrier_detect_changed();
}

ISR(USART_RXC_vect) {
    char c = UDR;
    if (bit_is_clear(UCSRA, FE)) {
        cmd_handler.input_buffer().push_back(c);
    }
}

void
init_uart(void) {
#define BAUD 38400
#include <util/setbaud.h>
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
#undef BAUD
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

    DEBUG_DDR |= _BV(DEBUG_BIT);
    DEBUG_PORT &= ~_BV(DEBUG_BIT);

    STATUS_LED_DDR |= _BV( STATUS_LED );

    init_uart();


    sei();
}

int main() {
    init();
    while ((volatile bool)true) {
        ui.update_led_state( model.socket_state() );

        uint8_t toggle_events = ui.poll_toggle_events();
        if (toggle_events) {
            relay_ctl.toggle( toggle_events );
        }

        cmd_handler.process_input();

        saver_.process();
    };
    return 0;
}
