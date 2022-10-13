#include <HardwareSerial.h>
//Libreria para el I2C
#include <Wire.h>

//Defincion del Accelerometro
#define ACC_FULL_SCALE_2G 0x00


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

  //TIMER
   timer=timerBegin(0,80,true); // Preescalado para pasar a 1MHz
   timerAttachInterrupt(timer,&onTimer, true);
   timerAlarmWrite(timer,1000000,true); //1millon/s, colocamos 1millon equivalente a 1s, true para que recargar automaticamente
   timerAlarmEnable(timer);

}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU,14); //Lectura de 14 bytes del MPU
  //Conversion de registros 
  ACC_X=(int16_t)(Wire.read()<<8|Wire.read())/16384.00;
  ACC_Y=(int16_t)(Wire.read()<<8|Wire.read())/16384.00;
  ACC_Z=(int16_t)(Wire.read()<<8|Wire.read())/16384.00;

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



