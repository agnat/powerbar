#include "model.hpp"

#include <string.h>
#include <avr/eeprom.h>

namespace powerbar {

uint8_t EEMEM eeprom_socket_state = 0;

char EEMEM eeprom_name_buffer[model::MAX_NAME_LENGTH * NUM_RELAYS];
uint8_t EEMEM eeprom_name_length[] = {0,0,0,0,0};

model::model() :
    socket_state_()
{
    socket_state_ = eeprom_read_byte( & eeprom_socket_state);
}

void
model::save_socket_state() {
    eeprom_write_byte( & eeprom_socket_state, socket_state_ );
}

uint8_t
model::get_name(uint8_t idx, char * dst, uint8_t max_length) const {
    uint8_t length = eeprom_read_byte( & eeprom_name_length[idx] );
    if (length == 0xff) {
        length = 0;
    }
    if (length >= max_length) {
        length = max_length - 1;
    }
    if (length > 0) {
        eeprom_read_block(dst, & eeprom_name_buffer[idx * MAX_NAME_LENGTH], length);
    }
    dst[length] = 0;
    return length;
}

void
model::set_name(uint8_t idx, const char * name) {
    uint8_t length = strlen( name );
    if (length > MAX_NAME_LENGTH) {
        length = MAX_NAME_LENGTH;
    }
    if (length > 0) {
        eeprom_write_block(name, & eeprom_name_buffer[idx * MAX_NAME_LENGTH], length);
    }
    eeprom_write_byte(& eeprom_name_length[idx], length);
}

} // end of namespace powerbar

