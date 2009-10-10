#ifndef POWERBAR_COMMAND_HANDLER_INCLUDED
#define POWERBAR_COMMAND_HANDLER_INCLUDED

#include "config.h"

#include "static_string.hpp"
#include "ring_buffer.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

inline
int
uart_putchar(char c/*, FILE *stream*/) {
    if (c == '\n') {
        uart_putchar('\r'/*, stream*/);
    }
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

namespace powerbar {

class command_handler {
    public:
        typedef etl::ring_buffer<char, 64> buffer_type;
        enum connection_state {
            DISCONNECTED,
            CONNECTED,
            DISCONNECTING,
        };
        
        command_handler();

        inline
        buffer_type &
        input_buffer() {
            return input_buffer_;
        }

        inline
        void
        process_input() {
            char c;
            cli();
            while ( ! input_buffer_.empty()) {
                c = input_buffer_.front();
                input_buffer_.pop_front();
                cmd_buffer_.append(c);
                switch(c) {
                    case '\n':
                        sei();
                        process_command();
                        cli();
                        break;
                }
            }
            sei();
        }

        inline
        void
        on_connection_established() {
            set_connection_state(CONNECTED);
        }

        inline
        void tick() {
            switch(connection_state_) {
                case CONNECTED:
                case DISCONNECTED:
                    break;
                case DISCONNECTING:
                    if (disconnect_timer_) {
                        --disconnect_timer_;
                    } else {
                        set_connection_state(DISCONNECTED);
                    }
                    break;
            }
        }
    private:
        enum { DISCONNECT_PULSE_LENGTH = 1 };

        inline
        void
        set_connection_state(connection_state new_state) {
            switch (new_state) {
                case CONNECTED:
                    STATUS_LED_PORT |= _BV(STATUS_LED);
                    break;
                case DISCONNECTING:
                    set_dtr(false);
                    disconnect_timer_ = DISCONNECT_PULSE_LENGTH;    
                    break;
                case DISCONNECTED:
                    set_dtr(true);
                    STATUS_LED_PORT &= ~_BV(STATUS_LED);
                    break;
            }
            connection_state_ = new_state;
        }
        inline
        void
        set_dtr(bool flag) {
            if (flag) {
                DTR_PORT |= _BV(DTR);
            } else {
                DTR_PORT &= ~_BV(DTR);
            }
        }

        inline
        void
        process_command() {
            if (cmd_buffer_ == "quit\r\n") {
                do_quit();
            }
            cmd_buffer_.clear();
        }

        inline
        void
        do_quit() {
            uart_putchar('q');
            uart_putchar('u');
            uart_putchar('i');
            uart_putchar('t');
            uart_putchar('.');
            uart_putchar('\n');
            set_connection_state(DISCONNECTING);
        }

        buffer_type input_buffer_;
        uint8_t disconnect_timer_;
        connection_state connection_state_;
        
        etl::static_string<char,128> cmd_buffer_;
};

}
#endif // POWERBAR_COMMAND_HANDLER_INCLUDED
