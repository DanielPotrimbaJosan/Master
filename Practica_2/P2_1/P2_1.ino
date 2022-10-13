#include <Arduino.h>
#include <FreeRTOSConfig.h>


//Definicion Variables
const int LedPin=5; //Salida del Led del ESP32
//Mediante porTick_Rate_MS estamos declarando la variable DelayLed que sera 200ms
const portTickType DelayLed =200 / portTICK_RATE_MS;


void setup() 
{
  //Comunicacion del puerto serie
  Serial.begin(112500);
  delay(100);

//Tarea,Nombre Tarea,Tamaño de la pila,Parametros Necesarios para la tarea,Prioridad,Argumentos
  xTaskCreate(ParpadeoLed, "ParpadeoLed",1000,NULL,1,NULL);
  xTaskCreate(Consola, "Consola",1000,NULL,1,NULL);

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
    //Encendemos el Led y despues de 200ms lo apagamos y asi en bucle
    digitalWrite(LedPin,HIGH);
    vTaskDelay(DelayLed);
    digitalWrite(LedPin,LOW);
    vTaskDelay(DelayLed);
  }
  //Es un bucle pero si por alguna razon se saliera, matamos la tarea.
  vTaskDelete(NULL);
}

//Generamos tarea 2
void Consola (void*ConsolaParametro)
{
  while(1)
  {
    //Se escribe Hola mundo por Consola cada 1 segundo
    Serial.println("Hola Mundo");
    vTaskDelay(DelayLed*5);
    
  }
  //Es un bucle pero si por alguna razon se saliera, matamos la tarea.
  vTaskDelete(NULL);
}

