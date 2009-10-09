#ifndef POWERBAR_COMMAND_HANDLER_INCLUDED
#define POWERBAR_COMMAND_HANDLER_INCLUDED

#include "ring_buffer.hpp"

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
        typedef ring_buffer<char, 64> buffer_type;
        
        inline
        buffer_type &
        input_buffer() {
            return input_buffer_;
        }

        inline
        void
        process_input() {
            char c;
            while ( ! input_buffer_.empty()) {
                c = input_buffer_.front();
                input_buffer_.pop_front();
                //uart_putchar(c);
            }
        }
    private:
        buffer_type input_buffer_;
};

}
#endif // POWERBAR_COMMAND_HANDLER_INCLUDED
