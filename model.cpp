#include "model.hpp"

#include <avr/eeprom.h>

namespace powerbar {

uint8_t EEMEM eeprom_socket_state = 0;

model::model() :
    socket_state_()
{
    socket_state_ = eeprom_read_byte( & eeprom_socket_state);
}

void
model::save_socket_state() {
    eeprom_write_byte( & eeprom_socket_state, socket_state_ );
}

} // end of namespace powerbar

