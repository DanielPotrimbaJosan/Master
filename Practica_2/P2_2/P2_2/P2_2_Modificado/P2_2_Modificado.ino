#include <HardwareSerial.h>
#include <MPU9250_asukiaaa.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

//Libreria para el I2C
#include <Wire.h>


#ifdef _ESP32_HAL_I2C_H_
//Definimos los pines del sensor MPU9265
#define SDA_PIN 21
#define SCL_PIN 22
#endif

//Asignacion del componente
  MPU9250_asukiaaa Misensor;


//Direccion del MPU
const int MPU=0x68;

//Declaramos las variables de tipo float
float ACC_X,ACC_Y,ACC_Z;

//Definicion Variables
const int LedPin=5; //Salida del Led del ESP32
//Mediante porTick_Rate_MS estamos declarando la variable DelayLed que sera 200ms
const portTickType DelayLed =200 / portTICK_RATE_MS;
const portTickType DelayConsola=1000 / portTICK_RATE_MS;




void setup() 
{
  //Comunicacion del puerto serie
  Serial.begin(112500);
  pinMode(LedPin,OUTPUT);

  #ifdef _ESP32_HAL_I2C_H_
  //Comunicacion por I2C por los pines asignados
  Wire.begin(SDA_PIN,SCL_PIN);
  Misensor.setWire(&Wire);
  #endif
  Misensor.beginAccel();
  
  delay(1000);
 

//Tarea,Nombre Tarea,Tamaño de la pila,Parametros Necesarios para la tarea,Prioridad,Argumentos
  xTaskCreate(Consola,"Consola",1000,NULL,1,NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
   //Almacenamos en las 3 variables generadas las posiciones del accelerometro
      Misensor.accelUpdate();
      ACC_X=Misensor.accelX();
      ACC_Y=Misensor.accelY();
      ACC_Z=Misensor.accelZ();
      delay(100);

}

//Generamos tarea 2
void Consola (void *ConsolaParametro)
{
  while(1)
  { 
    Serial.print("Acc X: ");
    Serial.println(ACC_X);
    Serial.print("Acc Y: ");
    Serial.println(ACC_Y);
    Serial.print("Acc Z: ");
    Serial.println(ACC_Z);
          
    //Generamos dentro de la tarea de Consola con prioridad 2(menor)
    xTaskCreate(ParpadeoLed, "ParpadeoLed",1000,NULL,2,NULL);
    vTaskDelay(DelayConsola);
  }
  //Es un bucle pero si por alguna razon se saliera, matamos la tarea.
  vTaskDelete(NULL);
}

//Generamos tarea 3
void ParpadeoLed (void *ledParametro)
{
    //Encendemos el Led y despues de 100ms lo apagamos y asi en bucle
    vTaskDelay(DelayConsola);
    digitalWrite(LedPin,HIGH);
    vTaskDelay(DelayLed);
    digitalWrite(LedPin,LOW);
    vTaskDelay(DelayLed);
    //Matamos la tarea con el EndLedTask
    vTaskDelete(NULL);
}

