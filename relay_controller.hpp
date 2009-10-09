#ifndef POWERBAR_RELAY_CONTROLLER_INCLUDED
#define POWERBAR_RELAY_CONTROLLER_INCLUDED

namespace powerbar {

class relay_controller {
    public:

        relay_controller();

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
                    if (relay_state_ != target_relay_state_) {
                        relays_to_toggle_ = relay_state_ ^ target_relay_state_;
                        current_bit_index_ = 0;
                        set_state(TRIGGER_PULSE);
                    }
                    break;
                case TRIGGER_PULSE:
                    if (target_relay_state_ == relay_state_) {
                        set_state(IDLE);
                    } else {
                        while ( ! bit_is_set(relays_to_toggle_, current_bit_index_)) {
                            ++current_bit_index_;
                        }
                        if (off_cycle_timers_[current_bit_index_] == 0) {
                            RELAY_PORT |= _BV(current_bit_index_) << 3;
                            pulse_timer_ = PULSE_WIDTH;
                            set_state(IN_PULSE);
                        }
                    }
                    break;
                case IN_PULSE:
                    if (pulse_timer_ == 0) {
                        uint8_t mask = _BV(current_bit_index_);
                        RELAY_PORT &= ~(mask << 3);
                        off_cycle_timers_[current_bit_index_] = MIN_OFF_CYCLE;
                        if (target_relay_state_ & mask) {
                            relay_state_ |= mask;
                        } else {
                            relay_state_ &= ~ mask;
                        }
                        relays_to_toggle_ &= ~mask;
                        set_state(TRIGGER_PULSE);
                    } else {
                        --pulse_timer_;
                    }
                    break;
            }
        }

        inline
        void
        set_all_relays(uint8_t mask) {
            while( ! is_idle() ) {};
            if (mask != relay_state_) {
                target_relay_state_ = mask;
            }
        }
        inline
        volatile bool
        is_idle() {
            return target_relay_state_ == relay_state_ && pulse_state_ == IDLE;
        }
        inline
        void
        toggle(uint8_t mask) {
            while( ! is_idle() ) {};
            set_all_relays( relay_state_ ^ mask);
        }
        inline
        uint8_t
        current_state() const {
            return relay_state_;
        }
    private:
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

        volatile PulseState pulse_state_;

        volatile uint8_t relay_state_;
        volatile uint8_t target_relay_state_;

        uint8_t current_bit_index_;
        uint8_t relays_to_toggle_;
        uint8_t pulse_timer_;
        uint8_t off_cycle_timers_[NUM_RELAYS];
};

}
#endif // POWERBAR_RELAY_CONTROLLER_INCLUDED
