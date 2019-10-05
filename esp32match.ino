#include <SPI.h>
#include <printf.h>
#include "RF24.h"
RF24 radio(7,8); //ce, csn
byte addressrx[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
byte ledstatus=0;
const int LedPin=9;
const int ControlPin=6;
const int RemoteControlPin=5;
const int pressDuration=2000;

void blink(void){ 
  delay(50);
  digitalWrite(LedPin,1);
  delay(500);
  digitalWrite(LedPin,0);
  delay(500);
  digitalWrite(LedPin,1);
  delay(500);
  digitalWrite(LedPin,0);
  delay(500);
  digitalWrite(LedPin,1);
  delay(500);  
  digitalWrite(LedPin,0);  
}

void setup() {
  pinMode(LedPin,OUTPUT); //LED pin
  pinMode(ControlPin,OUTPUT);  // Control Pin for Garage
  pinMode(RemoteControlPin,OUTPUT);  // Remote Control Pin for Big Gate
  digitalWrite(ControlPin,1);
  digitalWrite(RemoteControlPin,1);

  blink();
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MAX); //RF24_PA_MAX
  radio.setChannel(0x4c);
  radio.setCRCLength(RF24_CRC_8);
  radio.setDataRate(RF24_1MBPS); //RF24_250KBPS RF24_2MBPS
  radio.setAddressWidth(5);
  radio.setRetries(5,15);
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,addressrx);
  radio.startListening(); // Become a receiver.
  radio.printDetails();
} 
// End of setup

void loop()
{
  if (radio.available()){
    Serial.print("Size of payload is ");
    Serial.println(radio.getDynamicPayloadSize());
    while (radio.available()) {
      char buffer[33];
      uint8_t length = radio.getDynamicPayloadSize();
      //uint8_t length = 13;
      radio.read(buffer, length);
      buffer[length] = 0;
      Serial.print("Received: ");
      Serial.println(buffer);
      String ricevuto((char *)buffer);
      String passaON("Ciao: 4!");
      String passaOFF("Ciao: 6!");
      String passaApriSoloGrande("Ciao: 7!");
      String passaApriTutto("Ciao: 9!");
      if (!ricevuto.compareTo(passaON)){
        ledstatus=1;
        digitalWrite(ControlPin,0);
        delay(pressDuration);
        digitalWrite(ControlPin,1);
        blink();
        Serial.println("Match ok! Switching ON!!!!");        
        digitalWrite(LedPin,ledstatus);
      } else if (!ricevuto.compareTo(passaOFF)) {
        ledstatus=0; 
        digitalWrite(ControlPin,0);
        delay(pressDuration);
        digitalWrite(ControlPin,1);
        blink();
        Serial.println("Match ok! Switching OFF!!!!");        
        digitalWrite(LedPin,ledstatus);
      } 
      else if (!ricevuto.compareTo(passaApriSoloGrande)) {
        ledstatus=0; 
        digitalWrite(RemoteControlPin,0);
        delay(pressDuration);
        digitalWrite(RemoteControlPin,1);
        blink();
        Serial.println("Match ok! Opening Only Big Gate!!!!");        
        digitalWrite(LedPin,ledstatus);
      } else if (!ricevuto.compareTo(passaApriTutto)) {
        ledstatus=0; 
        digitalWrite(ControlPin,0);
        delay(pressDuration);
        digitalWrite(ControlPin,1);
        blink();
        delay(6999);
        digitalWrite(RemoteControlPin,0);
        delay(pressDuration);
        digitalWrite(RemoteControlPin,1);        
        blink();
        Serial.println("Match ok! Opening first Garage and then Big Gate!!!!");        
        digitalWrite(LedPin,ledstatus);
      } else {
        Serial.print("Sorry I only received: ");
        Serial.println(buffer);
        blink();
        digitalWrite(LedPin,ledstatus);
      }
    }
  }
}
