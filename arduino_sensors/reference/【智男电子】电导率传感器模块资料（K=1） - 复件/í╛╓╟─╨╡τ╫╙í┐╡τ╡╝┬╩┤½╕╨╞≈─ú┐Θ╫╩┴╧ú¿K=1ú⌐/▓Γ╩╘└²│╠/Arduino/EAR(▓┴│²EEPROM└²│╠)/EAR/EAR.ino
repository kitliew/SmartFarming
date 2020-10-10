
#include <EEPROM.h>
#define KVALUEADDR 0x0A
void setup(){
    for(byte i = 0;i< 8; i++ ){
      EEPROM.write(KVALUEADDR+i, 0xFF);
    }
}
void loop(){
}

