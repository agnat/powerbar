#ifndef POWERBAR_RELAY_CONTROLLER_INCLUDED
#define POWERBAR_RELAY_CONTROLLER_INCLUDED

#include "model.hpp"

namespace powerbar {

class relay_controller {
    public:

        relay_controller(model & m);

        inline
        void
        tick() {
            for (uint8_t i = 0; i < NUM_RELAYS; ++i) {
                if (off_cycle_timers_[i] > 0) {
                    --off_cycle_timers_[i];
                }
            }
            switch (pulse_state_) {
                case IDLE:
                    if (relays_to_toggle_) {
                        current_bit_index_ = 0;
                        set_state(TRIGGER_PULSE);
                    }
                    break;
                case TRIGGER_PULSE:
                    if (relays_to_toggle_) {
                        while ( ! bit_is_set(relays_to_toggle_, current_bit_index_)) {
                            ++current_bit_index_;
                        }
                        if (off_cycle_timers_[current_bit_index_] == 0) {
                            RELAY_PORT |= _BV(current_bit_index_) << 3;
                            pulse_timer_ = PULSE_WIDTH;
                            set_state(IN_PULSE);
                        }
                    } else {
                        set_state(IDLE);
                    }
                    break;
                case IN_PULSE:
                    if (pulse_timer_ == 0) {
                        uint8_t mask = _BV(current_bit_index_);
                        RELAY_PORT &= ~(mask << 3);
                        off_cycle_timers_[current_bit_index_] = MIN_OFF_CYCLE;
                        model_.on_relay_toggled(current_bit_index_);
                        relays_to_toggle_ &= ~mask;
                        set_state(TRIGGER_PULSE);
                    } else {
                        --pulse_timer_;
                    }
                    break;
            }
        }

        inline
        volatile bool
        is_idle() {
            return relays_to_toggle_ == 0 && pulse_state_ == IDLE;
        }
        inline
        void
        toggle(uint8_t mask) {
            while( ! is_idle() ) {};
            relays_to_toggle_ = mask;
        }
    private:
        relay_controller(relay_controller const& other);

        enum { 
            PULSE_WIDTH = 20,
            MIN_OFF_CYCLE = 180
        }; // ticks

        enum PulseState {
            IDLE,
            TRIGGER_PULSE,
            IN_PULSE
        };
        inline
        void
        set_state(PulseState theState) {
            pulse_state_ = theState;
        }

        model & model_;
        volatile PulseState pulse_state_;

        uint8_t current_bit_index_;
        volatile uint8_t relays_to_toggle_;
        uint8_t pulse_timer_;
        uint8_t off_cycle_timers_[NUM_RELAYS];
};

} // end of namespace powerbar
#endif // POWERBAR_RELAY_CONTROLLER_INCLUDED
