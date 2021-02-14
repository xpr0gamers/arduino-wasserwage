#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_24bargraph bar = Adafruit_24bargraph();

void setup() {
  Serial.begin(9600);
  Serial.flush();

  setupGradmesser();
  setupLedBar();
}


void loop() {
  int nGrad = getGrad();
  showLedBar(nGrad);
  delay(1000);
}

void setupLedBar(){
   bar.begin(0x70);  // pass in the address
}

void setupGradmesser(){
  Wire.begin();
  Wire.beginTransmission(0x68); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void showLedBar(int nGrad){
  Serial.println(nGrad);
  int nGrenzen[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48};
  int nElementCount = sizeof(nGrenzen) / sizeof(int);

  //alles ausschalten
  for(int i = 0; i < 24; i++){
    bar.setBar(i, LED_OFF);
  }

  //zwei Grüne in der Mitte immer anschalten
  bar.setBar(11, LED_GREEN);
  bar.setBar(12, LED_GREEN);
  
  if(nGrad > 0){
    for(int i = 1; i <= nElementCount; i++){
      int nFarbe;
      if(i < 6){
        nFarbe = LED_YELLOW;
      }else{
        nFarbe = LED_RED;
      }
      if(nGrad >= nGrenzen[i-1]){
        bar.setBar(11-i, nFarbe);
      }else{
        bar.setBar(11-i, LED_OFF);
      }
    }
  }else if(nGrad < 0){
    for(int i = 1; i <= nElementCount; i++){
      int nFarbe;
      if(i < 6){
        nFarbe = LED_YELLOW;
      }else{
        nFarbe = LED_RED;
      }
      if(nGrad * -1 >= nGrenzen[i-1]){
        bar.setBar(i + 12, nFarbe);
      }else{
        bar.setBar(i + 12, LED_OFF);
      }
    }
  }
  bar.writeDisplay();
}

int getGrad(){
  const int ACCEL_OFFSET   = 200;
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(0x68, 7*2, true); // request a total of 7*2=14 registers

  int accValue[3];
  for(byte i=0; i<3; i++) {
    accValue[i] = Wire.read()<<8 | Wire.read(); // reading registers: ACCEL_XOUT, ACCEL_YOUT, ACCEL_ZOUT
  }
  return constrain(map(accValue[1] - ACCEL_OFFSET, -16800, 16800, -90, 90), -90, 90); 
}
