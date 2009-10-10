#include "command_handler.hpp"

namespace powerbar {

command_handler::command_handler() :
    disconnect_timer_()
{
    set_connection_state(DISCONNECTED);
}

}
