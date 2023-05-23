/**
   @file RAK12033_6_Axis_BasicReadings_IIM_42652.ino
   @author rakwireless.com
   @brief  Get IIM-42652 sensor data and output data on the serial port.
   @version 0.1
   @date 2021-12-28
   @copyright Copyright (c) 2021
**/

#include "RAK12033-IIM42652.h"
#include <Wire.h>

IIM42652 IMU;
IIM42652_axis_t  accel_data;
IIM42652_axis_t  gyro_data;

const int local_char_variables_length = 9;

char ACC_X[local_char_variables_length];
char ACC_Y[local_char_variables_length];
char ACC_Z[local_char_variables_length];
char GYR_X[local_char_variables_length];
char GYR_Y[local_char_variables_length];
char GYR_Z[local_char_variables_length];
char TEMP[local_char_variables_length];

char cadena[128];
//10.10&11.11&12.12&20.20&21.21&22.22&33.33

void setup() 
{
  time_t timeout = millis();
  // Initialize Serial for debug output.
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println("RAK12033 Basic Reading example.");

  Wire.begin();
  if (IMU.begin() == false) 
  {
    while (1) 
    {
      Serial.println("IIM-42652 is not connected.");
      delay(5000);
    }
  }

  Serial.println("================================");
  Serial.println("Inicializar UART1");
  Serial.println("================================");

  Serial1.begin(115200, SERIAL_8N1);
  delay(1000);

  IMU.ex_idle();
  IMU.accelerometer_enable();
  IMU.gyroscope_enable();
  IMU.temperature_enable();
  
}

void six_axis() 
{
  
  float temp;

  float acc_x ,acc_y ,acc_z;
  float gyro_x,gyro_y,gyro_z;

  delay(100);
  
  IMU.get_accel_data(&accel_data );
  IMU.get_gyro_data(&gyro_data ); 
  IMU.get_temperature(&temp );

  /*
   * ±16 g  : 2048  LSB/g 
   * ±8 g   : 4096  LSB/g 
   * ±4 g   : 8192  LSB/g 
   * ±2 g   : 16384 LSB/g 
   */
  acc_x = (float)accel_data.x / 2048;
  acc_y = (float)accel_data.y / 2048;
  acc_z = (float)accel_data.z / 2048;
    
  Serial.print("Accel X: ");
  Serial.print(acc_x);
  Serial.print("[g]  Y: ");
  Serial.print(acc_y);
  Serial.print("[g]  Z: ");
  Serial.print(acc_z);
  Serial.println("[g]");

  /*
   * ±2000 º/s    : 16.4   LSB/(º/s)
   * ±1000 º/s    : 32.8   LSB/(º/s)
   * ±500  º/s    : 65.5   LSB/(º/s)
   * ±250  º/s    : 131    LSB/(º/s)
   * ±125  º/s    : 262    LSB/(º/s)
   * ±62.5  º/s   : 524.3  LSB/(º/s)
   * ±31.25  º/s  : 1048.6 LSB/(º/s) 
   * ±15.625 º/s  : 2097.2 LSB/(º/s)
   */
  gyro_x = (float)gyro_data.x / 16.4;
  gyro_y = (float)gyro_data.y / 16.4;
  gyro_z = (float)gyro_data.z / 16.4;
  
  Serial.print("Gyro  X:");
  Serial.print(gyro_x);
  Serial.print("º/s  Y: ");
  Serial.print(gyro_y);
  Serial.print("º/s  Z: ");
  Serial.print(gyro_z);
  Serial.println("º/s");

  Serial.print("Temper : ");
  Serial.print(temp);
  Serial.println("[ºC]");

  sprintf(ACC_X, "%.2f", acc_x);
  sprintf(ACC_Y, "%.2f", acc_y);
  sprintf(ACC_Z, "%.2f", acc_z);
  sprintf(GYR_X, "%.2f", gyro_x);
  sprintf(GYR_Y, "%.2f", gyro_y);
  sprintf(GYR_Z, "%.2f", gyro_z);
  sprintf(TEMP, "%.2f", temp);

  //IMU.accelerometer_disable();
  //IMU.gyroscope_disable();
  //IMU.temperature_disable();
  //IMU.idle();
}

void loop() {
  six_axis();
  memset(cadena, 0, sizeof(cadena));
  strcat(cadena, ACC_X);
  strcat(cadena, "&");
  strcat(cadena, ACC_Y);
  strcat(cadena, "&");
  strcat(cadena, ACC_Z);
  strcat(cadena, "&");
  strcat(cadena, GYR_X);
  strcat(cadena, "&");
  strcat(cadena, GYR_Y);
  strcat(cadena, "&");
  strcat(cadena, GYR_Z);
  strcat(cadena, "&");
  strcat(cadena, TEMP);
  
  Serial.println(cadena);
  Serial1.println(cadena);
  
  delay(4000);
}
