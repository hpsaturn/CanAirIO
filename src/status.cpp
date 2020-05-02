#include <status.hpp>

// Status flags byte
std::bitset<8> status;

int error_cycle;

void statusOn(int bit) {
    status.set(bit);
}

void statusOff(int bit) {
    status.reset(bit);
}

/***
 * Activate error code bits section with
 * the next possible errors:
 * ecode_sensor_ok          =   0;
 * ecode_sensor_read_fail   =   1;
 * ecode_sensor_timeout     =   2;
 * ecode_wifi_fail          =   3;
 * ecode_ifdb_write_fail    =   4;
 * ecode_ifdb_dns_fail      =   5;
 * ecode_json_parser_error  =   6;
 * ecode_invalid_config     =   7;
 * ecode_api_write_fail     =   8;
 **/
void setErrorCode(unsigned int error) {
    status = ((status.to_ulong() & ~0xf0) | ((error << 4) & 0xf0));
}

unsigned int getErrorCode() {
    return (unsigned int)(status.to_ulong() >> 4);
}

/***
 * Clear status error code after MAX_ERROR_LIFE_CYCLE
 **/
void updateStatusError() {
    if (error_cycle++ == MAX_ERROR_LIFE_CYCLE) {
        setErrorCode(0);
        error_cycle = 0;
    }
}

const char* getStatus() {
  return status.to_string().c_str();
}
