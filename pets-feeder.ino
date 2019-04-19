#include <Wire.h>
#include "RTClib.h" //https://github.com/adafruit/RTClib

RTC_DS1307 rtc;

#define FIRST  8
#define SECOND 9

#define COUNT  2
#define VALUES_COUNT  2

union ArrayToInteger {
 uint32_t integer;
 byte array[4];
};

byte states[COUNT];
byte pins[COUNT] = {FIRST, SECOND};
byte values[VALUES_COUNT] = {HIGH, LOW};
char* names[COUNT] = {"cat", "dog"};
char* valuesStr[COUNT] = {"false", "true"};
uint32_t times[COUNT][VALUES_COUNT];
char* timesValuesStr[VALUES_COUNT] = {"turnOffTime", "turnOnTime"};
int i, j, offset, l;
ArrayToInteger converter;

DateTime now;

void setup()
{
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  for (i = 0; i < COUNT; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], values[states[i]]);

    for (j = 0; j < VALUES_COUNT; j++) {
      readTime(i, j);
    }
  }

  Serial.begin(9600);
}
void loop()
{
  now = rtc.now();
  
  printStates();
  handleCommands();
  delay(1000);
}


void printStates() {
  Serial.print("{");
  for (i = 0; i < COUNT; i++) {
    Serial.print("\"");
    Serial.print(names[i]);
    Serial.print("\":{\"on\":");
    Serial.print(valuesStr[states[i]]);
    for (j = 0; j < COUNT; j++) {
      Serial.print(",\"");
      Serial.print(timesValuesStr[j]);
      Serial.print("\":");
      Serial.print(times[i][j], DEC);
      Serial.print("000");
    }
    Serial.print("}");
    if (i < COUNT - 1) {
      Serial.print(",");
    }
  }
  Serial.println("}");
}

void handleCommands() {
 while (Serial.available() > 0) {
   char a = Serial.read();
   byte c = (byte) a - 48;

   if (c < 0 || c >= COUNT * VALUES_COUNT) {
     continue;
   }
   
   byte address = c / VALUES_COUNT;
   byte value = c % VALUES_COUNT;

   if (states[address] == value) {
     continue;
   }
   
   states[address] = value;
   digitalWrite(pins[address], values[states[address]]);
   writeTime(address, value, now.unixtime());
 }
}

void writeTime(byte address, byte value, uint32_t currentTime) {
  times[address][value] = currentTime;
  converter.integer = currentTime;
  offset = address * 16 + value * 4;
  for (l = 0; l < 4; l++) {
    rtc.writenvram(offset + l, converter.array[l]);
  }
}

void readTime(byte address, byte value) {
  offset = address * 16 + value * 4;
  for (l = 0; l < 4; l++) {
    converter.array[l] = rtc.readnvram(offset + l);
  }
  times[address][value] = converter.integer;
}




