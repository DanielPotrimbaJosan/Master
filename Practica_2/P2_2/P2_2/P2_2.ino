#include <Arduino.h>
#include <FreeRTOSConfig.h>
#include <HardwareSerial.h>
//Libreria para el I2C
#include <Wire.h>


//Defincion del Accelerometro
#define ACC_FULL_SCALE_2G 0x00
//Direccion del MPU
const int MPU=0x68;
//Declaramos las variables como doble int
double ACC_X,ACC_Y,ACC_Z;


//Definicion Variables
const int LedPin=5; //Salida del Led del ESP32
//Mediante porTick_Rate_MS estamos declarando la variable DelayLed que sera 200ms
const portTickType DelayLed =200 / portTICK_RATE_MS;
const portTickType DelayConsola=1000 / portTICK_RATE_MS;
const portTickType DelayAcc=100 / portTICK_RATE_MS;



void setup() 
{
  //Comunicacion del puerto serie
  Serial.begin(112500);
  delay(100);
   //Comunicacion I2C
  Wire.begin();
  //Velocidad de comunicacion del puerto I2C
  Wire.setClock(100000);
  delay(100);
  //Encedemos el MPU9265 donde esta el acelerometro
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // Power register
  Wire.write(0);     // Ponemos a 0 para despertar el MPU-6050
  Wire.endTransmission();
  delay(100);

  //Configuramos el Accelerometro
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);
  Wire.write(ACC_FULL_SCALE_2G);
  Wire.endTransmission();
  delay(100);

//Tarea,Nombre Tarea,Tamaño de la pila,Parametros Necesarios para la tarea,Prioridad,Argumentos
  xTaskCreate(ParpadeoLed, "ParpadeoLed",1000,NULL,1,NULL);
  xTaskCreate(Acelerometro, "Acelerometro",1000,NULL,2,NULL);
  xTaskCreate(Consola,"Consola",1000,NULL,1,NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);

}

//Generamos tarea 1
void ParpadeoLed (void *ledParametro)
{
  pinMode(LedPin,OUTPUT);
  while(1)
  { 
    //Encendemos el Led y despues de 100ms lo apagamos y asi en bucle
    digitalWrite(LedPin,HIGH);
    vTaskDelay(DelayLed);
    digitalWrite(LedPin,LOW);
    vTaskDelay(DelayLed);
    
    
  }
  //Es un bucle pero si por alguna razon se saliera, matamos la tarea.
  vTaskDelete(NULL);
}

//Generamos tarea 2
void Acelerometro (void*AcelerometroParametro)
{
  while(1)
  {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU,14); //Lectura de 14 bytes del MPU
  //Conversion de registros 
  ACC_X=(int16_t)(Wire.read()<<8|Wire.read())/16384.00;
  ACC_Y=(int16_t)(Wire.read()<<8|Wire.read())/16384.00;
  ACC_Z=(int16_t)(Wire.read()<<8|Wire.read())/16384.00;
  //Cada 100ms leemos los valores
  vTaskDelay(DelayAcc);
    
  }
  //Es un bucle pero si por alguna razon se saliera, matamos la tarea.
  vTaskDelete(NULL);
}
//Generamos tarea 3
void Consola (void *ConsolaParametro)
{
  pinMode(LedPin,OUTPUT);
  while(1)
  { 
    Serial.print("Acc X: ");
    Serial.println(ACC_X);
    Serial.print("Acc Y: ");
    Serial.println(ACC_Y);
    Serial.print("Acc Z: ");
    Serial.println(ACC_Z);
    vTaskDelay(DelayConsola);
  }
  //Es un bucle pero si por alguna razon se saliera, matamos la tarea.
  vTaskDelete(NULL);
}

