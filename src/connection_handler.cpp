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
    //set_connection_state(DISCONNECTED); // keep TCP connection through MCU resets
    set_connection_state(DISCONNECTING); // pulse DTR to quit TCP connection
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
    } else if (cmd_buffer_.find_P(PSTR("rename")) == 0) {
        do_rename();
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

PGM_P
state_string(bool is_on) {
    return is_on ? PSTR("on") : PSTR("off");
}

void
connection_handler::print_status_line() {
    uint8_t sockets = model_.socket_state();
    for (uint8_t i = 0; i < NUM_RELAYS; ++i) {
        fprintf_P(&cout_, PSTR("%d: %S "), i+1, state_string(sockets & _BV(i)));
    }
    fputc('\n', &cout_);
}

inline
void
connection_handler::do_toggle() {
    int indices[NUM_RELAYS];
    int num_items = sscanf_P(cmd_buffer_.c_str(),
            PSTR("toggle %d %d %d %d %d"),
            &indices[0], &indices[1], &indices[2], &indices[3], &indices[4]);
    if (num_items > 0) {
        bool cmd_ok = true;
        uint8_t relays = 0;
        for (int i = 0; i < num_items; ++i) {
            if (0 < indices[i] && indices[i] <= NUM_RELAYS) {
                relays |= _BV(indices[i]-1);
            } else {
                fprintf_P(&cout_, 
                        PSTR("error: index %d is out of range\n"), indices[i]);
                cmd_ok = false;
                break;
            }
        }
        if (cmd_ok) {
            relay_controller_.toggle(relays);
            while ( ! relay_controller_.is_idle() ) {}
            print_status_line();
        }
    } else {
        char name[33];
        num_items = sscanf_P(cmd_buffer_.c_str(), PSTR("toggle %32s"), &name);
        if (num_items == 1) {
            if (strcmp_P(name, PSTR("all")) == 0) {
                relay_controller_.toggle(0x1f);
                while ( ! relay_controller_.is_idle() ) {}
                print_status_line();
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
    char name_buffer[model::MAX_NAME_LENGTH];
    for (uint8_t i = 0; i < NUM_RELAYS; ++i) {
        model_.get_name(i, name_buffer, model::MAX_NAME_LENGTH);
        fprintf_P(&cout_, PSTR("%d: %s %S\n"), i+1, name_buffer,
                state_string(_BV(i) & model_.socket_state()));
    }
}

inline
void
connection_handler::do_rename() {
    char name_buffer[model::MAX_NAME_LENGTH];
    uint8_t index;
    int num_items = sscanf_P(cmd_buffer_.c_str(),
            PSTR("rename %d %s"),
            &index, & name_buffer);
    if (num_items == 2 && index > 0 && index <= NUM_RELAYS) {
        fprintf_P(&cout_,PSTR("renaming %d to %s\n"), index, name_buffer);
        model_.set_name(index - 1, name_buffer);
    }
}

} // end of namespace powerbar
