#include <HardwareSerial.h>

//Libreria para el I2C
#include <Wire.h>

#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
//Definimos los pines del sensor MPU9265
#define SDA_PIN 21
#define SCL_PIN 22
#endif

//Asignacion del componente
MPU9250_asukiaaa Misensor;

//Direccion del MPU
const int MPU=0x68;
//Declaramos las variables como doble int
double ACC_X,ACC_Y,ACC_Z;


//Salida del Led
const int LedPin= 5;

//Valor Entero del valor para el temporiador
int X_Entero;
int X_seg=0;


void setup() {
  //Comunicacion del puerto serie
  Serial.begin(115200); 
  pinMode(LedPin,OUTPUT);
 
  #ifdef _ESP32_HAL_I2C_H_
  //Comunicacion por I2C por los pines asignados
  Wire.begin(SDA_PIN,SCL_PIN);
  Misensor.setWire(&Wire);
  #endif
  Misensor.beginAccel();

  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:
 //Falta temporizar 1 segundo la salida por consola
  Misensor.accelUpdate();
  ACC_X=Misensor.accelX();
  ACC_Y=Misensor.accelY();
  ACC_Z=Misensor.accelZ();
  delay(100);
          
          Serial.print("Acc X: ");
          Serial.println(ACC_X);
          Serial.print("Acc Y: ");
          Serial.println(ACC_Y);
          Serial.print("Acc Z: ");
          Serial.println(ACC_Z);
          

          digitalWrite(LedPin, HIGH);
          delay(200);
          digitalWrite(LedPin,LOW);
          delay(200);
  

    

  
  }



