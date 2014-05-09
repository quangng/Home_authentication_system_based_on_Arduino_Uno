#ifndef test_new_lib_H_
#define test_new_lib_H_
#include "Arduino.h"
#include "SIM900.h"
#include "sms.h"
#include "call.h"
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//Function prototypes
void serialhwread(void);
void serialswread(void);
void handleLCDDisplay(uint8_t message_length, const char *msg);
void handleCall(void);
void handleSMS(void);




#endif /* test_new_lib_H_ */
