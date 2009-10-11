#ifndef POWERBAR_CONNECTION_HANDLER_INCLUDED
#define POWERBAR_CONNECTION_HANDLER_INCLUDED

#include "config.h"

#include "static_string.hpp"
#include "ring_buffer.hpp"
#include "relay_controller.hpp"
#include "model.hpp"

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

namespace powerbar {

enum { NUM_COMMANDS = 2 };

class connection_handler {
    public:
        typedef etl::ring_buffer<char, 64> buffer_type;
        enum connection_state {
            DISCONNECTED,
            CONNECTED,
            DISCONNECTING,
        };
        
        enum session_type {
            UNDECIDED,
            CLI_SESSION,
            HTTP_SESSION
        };
        connection_handler(model & m, relay_controller & rc);

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

                sei();
                
                cmd_buffer_.append(c);
                switch (session_type_) {
                    case UNDECIDED:
                        if ( c == '\n') {
                            if (is_http_request()) {
                                session_type_ = HTTP_SESSION;
                                handle_http_request(c);
                            } else {
                                session_type_ = CLI_SESSION;
                                handle_cli_session(c);
                            }
                        }
                        break;
                    case CLI_SESSION:
                        handle_cli_session(c);
                        break;
                    case HTTP_SESSION:
                        handle_http_request(c);
                        break;
                }
                
                cli();
            }
            sei();
        }
        inline
        void
        on_carrier_detect_changed() {
            if (connection_state_ == CONNECTED) {
                set_connection_state(DISCONNECTED);
                
            } else {
                set_connection_state(CONNECTED);
            }
        }

        inline
        void tick() {
            switch(connection_state_) {
                case CONNECTED:
                case DISCONNECTED:
                    break;
                case DISCONNECTING:
                    if (dtr_pulse_timer_) {
                        --dtr_pulse_timer_;
                    } else {
                        set_connection_state(DISCONNECTED);
                    }
                    break;
            }
        }
    private:
        enum { DTR_PULSE_LENGTH = 1 };

        inline
        bool
        is_http_request() {
            return false;
        }

        void
        handle_cli_session(char c);

        inline
        void
        handle_http_request(char c) {
        }


        inline
        void
        set_connection_state(connection_state new_state) {
            switch (new_state) {
                case CONNECTED:
                    // configure  carrier detect pin (DCD) to trigger on a rising edge
                    GICR &= ~_BV(INT2);
                    MCUCSR |= _BV(ISC2); // trigger on rising edge
                    GICR |= _BV(INT2);
                    GIFR |= _BV(INTF2);
                    STATUS_LED_PORT |= _BV(STATUS_LED);
                    break;
                case DISCONNECTING:
                    set_dtr(false);
                    dtr_pulse_timer_ = DTR_PULSE_LENGTH;    
                    break;
                case DISCONNECTED:
                    // configure  carrier detect pin (DCD) to trigger on a falling edge
                    GICR &= ~_BV(INT2);
                    MCUCSR &= ~_BV(ISC2); // trigger on falling edge
                    GICR |= _BV(INT2);
                    GIFR |= _BV(INTF2);

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

        void process_command();

        inline void do_quit();
        inline void do_toggle();
        inline void do_status();

        model &          model_;
        buffer_type      input_buffer_;
        uint8_t          dtr_pulse_timer_;
        connection_state connection_state_;
        session_type     session_type_;

        relay_controller & relay_controller_;
        
        etl::static_string<char,128> cmd_buffer_;

        FILE cout_;
};

} // end of namespace powerbar
#endif // POWERBAR_CONNECTION_HANDLER_INCLUDED
