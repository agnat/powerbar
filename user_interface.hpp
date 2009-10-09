#ifndef POWERBAR_USER_INTERFACE_INCLUDED
#define POWERBAR_USER_INTERFACE_INCLUDED

#include "config.h"

#include <avr/interrupt.h>

namespace powerbar {

class user_interface {
    public:
        user_interface();

        inline
        void
        tick() {
            uint8_t toggle_state = (~(uint8_t)TOGGLE_PIN) >> 3;
            for (uint8_t i = 0; i < NUM_RELAYS; ++i) {
                if ( bit_is_set( toggle_state, i)) {
                    if (toggle_pressed_counter_[i] < MIN_PRESSED_CYCLES) {
                        ++toggle_pressed_counter_[i];
                    }
                } else {
                    toggle_pressed_counter_[i] = 0;
                    already_toggled_ &= ~_BV(i);
                }
                if (toggle_pressed_counter_[i] == MIN_PRESSED_CYCLES &&
                    ! bit_is_set(already_toggled_,i))
                {
                    events_ |= _BV(i);
                    already_toggled_ |= _BV(i);
                }
            }
        }
        inline
        uint8_t
        poll_toggle_events() {
            cli();
            uint8_t e( events_ );
            events_ = 0;
            sei();
            return e;
        }
        inline
        void
        update_led_state(uint8_t led_state) {
            LED_PORT = (led_state << 3) | (LED_PIN & 0x7);
        }

    private:
        enum { MIN_PRESSED_CYCLES = 50 };
        uint8_t toggle_pressed_counter_[NUM_RELAYS];
        uint8_t already_toggled_;

        volatile uint8_t events_;
};

}

#endif // POWERBAR_USER_INTERFACE_INCLUDED
