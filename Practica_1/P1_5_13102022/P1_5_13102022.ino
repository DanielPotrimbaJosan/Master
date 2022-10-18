#include <HardwareSerial.h>

//Libreria para el I2C
#include <Wire.h>

#include <MPU9250_asukiaaa.h>
//Definimos los pines del sensor MPU9265
#define SDA_PIN 21
#define SCL_PIN 22

//Asignacion del componente
MPU9250_asukiaaa Misensor;

//Direccion del MPU
const int MPU=0x68;
//Declaramos las variables como doble int
double ACC_X,ACC_Y,ACC_Z;

//Generamos un contador para la interrupcion
volatile int ContadorInterrupcion; 

//Salida del Led
const int LedPin= 5;

//Valor Entero del valor para el temporiador
int X_Entero;
int X_seg=0;


//Timer con puntero a esta variable
hw_timer_t * timer = NULL;

// Se sincroniza el bucle principar con el ISR"Interrupt Service Routine"
portMUX_TYPE timerMux =portMUX_INITIALIZER_UNLOCKED;

// Para indicar que se ha generado una interrupcion
void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
    ContadorInterrupcion++;
  portEXIT_CRITICAL_ISR(&timerMux);
}


void setup() {
  //Comunicacion de la UART
  Serial.begin(115200); 
  pinMode(LedPin,OUTPUT);
  //Comunicacion por I2C por los pines asignados
  Wire.begin(SDA_PIN,SCL_PIN);
  Misensor.setWire(&Wire);

  delay(100);

  //TIMER
   timer=timerBegin(0,80,true); // Preescalado para pasar a 1MHz
   timerAttachInterrupt(timer,&onTimer, true);
   timerAlarmWrite(timer,1000000,true); //1millon/s, colocamos 1millon equivalente a 1s, true para que recargar automaticamente
   timerAlarmEnable(timer);

}

void loop() {
  // put your main code here, to run repeatedly:
  Misensor.accelUpdate();
  ACC_X=Misensor.accelX();
  ACC_Y=Misensor.accelY();
  ACC_Z=Misensor.accelZ();

   timerAlarmWrite(timer,X_seg,true);
   timerAlarmEnable(timer);
      if (ContadorInterrupcion > 0)
        {
          portENTER_CRITICAL(&timerMux);
          ContadorInterrupcion--;
          portEXIT_CRITICAL(&timerMux);

          ContadorInterrupcion++;

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

    

  
  }



