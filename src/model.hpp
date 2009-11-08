#ifndef POWERBAR_MODEL_INCLUDED
#define POWERBAR_MODEL_INCLUDED

#include "config.h"

#include <inttypes.h>
#include <avr/io.h>

namespace powerbar {

class model {
    public:
        model();

        enum {
            MAX_NAME_LENGTH = 32
        };

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

        uint8_t
        get_name(uint8_t idx, char * dst, uint8_t max_length) const;
        void
        set_name(uint8_t idx, const char * name);

        void
        save_socket_state();
    private:
        volatile uint8_t socket_state_;

};

} // end of namespace powerbar
#endif // POWERBAR_MODEL_INCLUDED
