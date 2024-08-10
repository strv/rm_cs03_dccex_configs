// ESP32 ADC
// Vref = 1.1 * 11dB = 3.9V
// Resolution = 4096
// I2V Gain = 2.2 V / A
// V2I Gain = 1/2.5 A/V
// ADC2V Gain = 3.9 / 4095
// ADC2I Gain = 3.9 / 4095 * 1 / 2.2
// senseFactor = ADC2I * 1000 = 0.43322758148178266178266178266178

#define RM_CS03 F("RM_CS03"), \
                  new MotorDriver(22, 19, UNUSED_PIN, -23, 33, 0.43322758148178266178266178266178, 1000, 21)
#define MOTOR_SHIELD_TYPE RM_CS03

#define IP_PORT 2560

#define ENABLE_WIFI true
#define WIFI_SSID "Your network name"
#define WIFI_PASSWORD "Your network passwd"
#define WIFI_HOSTNAME "dccex"
#define WIFI_CHANNEL 1
#define WIFI_FORCE_AP false
// #define ENABLE_ETHERNET true
// #define IP_ADDRESS { 192, 168, 1, 200 }
#define DISABLE_EEPROM
// #define DISABLE_PROG
#define DIAG_IO
