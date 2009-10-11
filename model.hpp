#ifndef POWERBAR_MODEL_INCLUDED
#define POWERBAR_MODEL_INCLUDED

#include <inttypes.h>
#include <avr/io.h>

namespace powerbar {

class model {
    public:
        model();

        inline
        uint8_t
        socket_state() const {
            return socket_state_;
        }

        inline
        void
        on_relay_toggled(uint8_t idx) {
            socket_state_ ^= _BV(idx);
        }

        void
        save_socket_state();
    private:
        volatile uint8_t socket_state_;

};

} // end of namespace powerbar
#endif // POWERBAR_MODEL_INCLUDED
