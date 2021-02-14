#include <Wire.h>

#define MPU6050_ADRESS 0x68

const int ACCEL_OFFSET   = 200;
const float LOOP_TIME    = 1; // 0.1 = 100ms
int accValue[3];


void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADRESS); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Serial.begin(9600);
  Serial.flush();
}
void loop()
{
  Wire.beginTransmission(MPU6050_ADRESS);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU6050_ADRESS, 7*2, true); // request a total of 7*2=14 registers

  for(byte i=0; i<3; i++) {
    accValue[i] = Wire.read()<<8 | Wire.read(); // reading registers: ACCEL_XOUT, ACCEL_YOUT, ACCEL_ZOUT
  }

  Serial.print("Grad: ");
  int nGrad = constrain(map(accValue[1] - ACCEL_OFFSET, -16800, 16800, -90, 90), -90, 90); 
  Serial.println(nGrad);
  delay(LOOP_TIME * 1000);
}
