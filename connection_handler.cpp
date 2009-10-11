#include "connection_handler.hpp"

int
uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

namespace powerbar {

connection_handler::connection_handler(model & m, relay_controller & rc) :
    model_(m),
    dtr_pulse_timer_(),
    session_type_(UNDECIDED),
    relay_controller_(rc)
{
    cout_.put = uart_putchar;
    cout_.get = NULL;
    cout_.flags = _FDEV_SETUP_WRITE;
    cout_.udata = 0;

    // configure DTR pin
    DTR_DDR  |= _BV(DTR);
    set_connection_state(DISCONNECTED);
}

void
connection_handler::process_command() {
    if ((cmd_buffer_.find_P(PSTR("quit")) == 0) ||
        (cmd_buffer_.find_P(PSTR("exit")) == 0))
    {
        do_quit();
    } else if (cmd_buffer_.find_P(PSTR("toggle")) == 0) {
        do_toggle();
    } else if (cmd_buffer_.find_P(PSTR("status")) == 0) {
        do_status();
    } else if (cmd_buffer_.find_P(PSTR("\r\n")) == 0) {
    } else {
        fputs_P(PSTR("error: unknown command.\n"), &cout_);
    }
    cmd_buffer_.clear();
}

void
connection_handler::handle_cli_session(char c) {
    if (cmd_buffer_.full()) {
        fputs_P(PSTR("error: maximum command line length exceeded\n"), &cout_);
        cmd_buffer_.clear();
    } else {
        switch(c) {
            case '\n':
                process_command();
                break;
        }
    }
}

inline
void
connection_handler::do_quit() {
    fputs_P(PSTR("bye.\n"), &cout_);
    set_connection_state(DISCONNECTING);
}

inline
PGM_P
state_string(bool is_on) {
    return is_on ? PSTR("on") : PSTR("off");
}

inline
void
connection_handler::do_toggle() {
    int idx;
    int result = sscanf_P(cmd_buffer_.c_str(), PSTR("toggle %d"), & idx);
    if (result == 1) {
        if (0 < idx && idx <= NUM_RELAYS) {
            relay_controller_.toggle(_BV(idx-1));
            while ( ! relay_controller_.is_idle() ) {}
            fprintf_P(&cout_, PSTR("%d is %S\n"), idx,
                    state_string(model_.socket_state() & _BV(idx-1)));
        } else {
            fputs_P(PSTR("error: index out of range\n"), &cout_);
        }
    } else {
        char name[33];
        result = sscanf_P(cmd_buffer_.c_str(), PSTR("toggle %32s"), &name);
        if (result == 1) {
            if (strcmp_P(name, PSTR("all")) == 0) {
                relay_controller_.toggle(0x1f);
            } else {
                fputs_P(PSTR("error: unknown socket name\n"), &cout_);
            }
        } else {
            fputs_P(PSTR("error: syntax error\n"), &cout_);
        }
    }
}

inline
void
connection_handler::do_status() {
    if(cmd_buffer_.size() == 8) {
        for (uint8_t i = 0; i < NUM_RELAYS; ++i) {
            fprintf_P(&cout_, PSTR("%d: %S\n"), i+1,
                    state_string(_BV(i) & model_.socket_state()));
        }
    }
}

} // end of namespace powerbar
