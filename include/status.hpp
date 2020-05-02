#include <bitset>

#define MAX_ERROR_LIFE_CYCLE 5

#define bit_sensor 0  // sensorr
#define bit_paired 1  // bluetooth
#define bit_wan 2     // internet access
#define bit_cloud 3   // publish cloud
#define bit_code0 4   // error code bit 0
#define bit_code1 5   // error code bit 1
#define bit_code2 6   // error code bit 2
#define bit_code3 7   // error code bit 3

#define ecode_sensor_ok 0
#define ecode_sensor_read_fail 1
#define ecode_sensor_timeout 2
#define ecode_wifi_fail 3
#define ecode_ifdb_write_fail 4
#define ecode_ifdb_dns_fail 5
#define ecode_json_parser_error 6
#define ecode_invalid_config 7
#define ecode_api_write_fail 8

void statusOn(int bit);
void statusOff(int bit);
void setErrorCode(unsigned int error);
unsigned int getErrorCode();
void updateStatusError();
const char* getStatus();