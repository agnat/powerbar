#include "user_interface.hpp"

namespace powerbar {

user_interface::user_interface() :
    toggle_pressed_counter_(),
    already_toggled_(),
    events_()
{
    TOGGLE_DDR &= ~ALL_BITS;
    TOGGLE_PORT |= ALL_BITS;

    LED_DDR |= ALL_BITS;
    LED_PORT &= ~ALL_BITS;
}

}
