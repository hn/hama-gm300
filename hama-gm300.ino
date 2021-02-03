/*
  Simple example for receiving and decoding hama GM-300 measurements

  https://github.com/hn/hama-gm300/
  https://github.com/sui77/rc-switch/
*/

#include "RCSwitch.h" // local 64bit version

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  delay(500);
  Serial.println("Starting hama GM-300 garden monitor decoder");
}

bool decodeHamaGM300(unsigned long long ookbits, unsigned int &hostid, unsigned int &chan,
                     bool &manual, bool &lowbat, unsigned int &humidity, unsigned int &soiltemp, unsigned int &airtemp) {

  uint8_t crc = 0xff;

  hostid = 0;
  chan = 0;
  humidity = 0;
  soiltemp = 0;
  airtemp = 0;
  manual = false;
  lowbat = false;

  for (uint8_t i = 8 * 7; i; i -= 8) {
    crc ^= ookbits >> i;
    for (uint8_t j = 0; j < 8; j++) {
      crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
  }
  if ((uint8_t) ookbits != crc) {
    return false;
  }

  // HHHHHHHHHHHH-CCC-MB-HH-U-SSSSSSSSSSSS-AAAAAAAAAAAA-UUUUUUUUUUUU-RRRRRRRR
  for (uint8_t i = 63; i > 19; i--) {
    if (i > 51 && (ookbits & (1ULL << i))) {
      hostid |= (1 << (63 - i));
    } else if (i <= 51 && i > 48 && (ookbits & (1ULL << i))) {
      chan |= (1 << (51 - i));
    } else if (i <= 46 && i > 44 && (ookbits & (1ULL << i))) {
      humidity |= (1 << (46 - i));
    } else if (i <= 43 && i > 31 && (ookbits & (1ULL << i))) {
      soiltemp |= (1 << (43 - i));
    } else if (i <= 31 && (ookbits & (1ULL << i))) {
      airtemp |= (1 << (31 - i));
    }
  }
  if (ookbits & (1ULL << 48))
    manual = true;
  if (ookbits & (1ULL << 47))
    lowbat = true;

  return true;
}

void loop() {
  if (mySwitch.available()) {
    unsigned long long value = mySwitch.getReceivedValue();

    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print((unsigned long)(value >> 32), 16);
      Serial.print((unsigned long)value, 16);
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );

      if (mySwitch.getReceivedBitlength() == 64) {
        unsigned int hostid, chan, humidity, soiltemp, airtemp;
        bool manual, lowbat;

        if (decodeHamaGM300(value, hostid, chan, manual, lowbat, humidity, soiltemp, airtemp)) {
          Serial.print("hostid:   "); Serial.println(hostid);
          Serial.print("channel:  "); Serial.println(chan);
          Serial.print("manual:   "); Serial.println(manual);
          Serial.print("lowbat:   "); Serial.println(lowbat);
          Serial.print("humidity: "); Serial.println(humidity);
          Serial.print("soiltemp: "); Serial.println(soiltemp);
          Serial.print("airtemp:  "); Serial.println(airtemp);
        } else {
          Serial.println("Received non-conforming 64bit telegram");
        }
      }

      mySwitch.resetAvailable();
    }
  }

}
