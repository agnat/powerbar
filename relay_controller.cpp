#include "config.h"

#include "relay_controller.hpp"

namespace powerbar {

relay_controller::relay_controller() :
    pulse_state_(IDLE),
    relay_state_(),
    target_relay_state_(),
    current_bit_index_(),
    relays_to_toggle_(),
    pulse_timer_(),
    off_cycle_timers_()
{
    // configure relay pins as outputs
    RELAY_DDR |= ALL_BITS;

}

}
