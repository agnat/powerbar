#ifndef POWERBAR_SAVE_TIMER_INCLUDED
#define POWERBAR_SAVE_TIMER_INCLUDED

#include "config.h"

#include "model.hpp"

namespace powerbar {

class save_timer {
    public:
        save_timer(model & m, relay_controller & rc) :
            model_(m),
            relay_controller_(rc),
            saved_state_(m.socket_state()),
            previous_state_(m.socket_state())
        {
        }

        inline
        void
        tick() {
            if (timer_) {
                --timer_;
            }
        }
        inline
        void
        process() {
            uint8_t state = model_.socket_state();
            if (previous_state_ != state) {
                cli();
                timer_ = STABELIZE_TIMER;
                sei();
                previous_state_ = state;
            }
            cli();
            bool timer_expired = timer_ == 0;
            sei();
            if (state != saved_state_ && timer_expired && relay_controller_.is_idle()) {
                model_.save_socket_state();
                saved_state_ = previous_state_ = model_.socket_state();
            }

        }
    private:
        enum { STABELIZE_TIMER = 1000 };
        model & model_;
        relay_controller & relay_controller_;
        uint8_t saved_state_;
        uint8_t previous_state_;
        volatile unsigned timer_;
};

} // end of namespace powerbar
#endif // POWERBAR_SAVE_TIMER_INCLUDED
