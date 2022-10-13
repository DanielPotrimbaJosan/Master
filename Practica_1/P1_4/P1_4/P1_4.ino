#include <HardwareSerial.h>

//Tenemos Conectado el Potenciometro al pin 34 "GPIO34"
int Epotenciometro=34;
//Salida del PWM al led del GPIO5
const int LedPin=5;

//Generamos un contador para la interrupcion
volatile int ContadorInterrupcion; 

//Generamos variable para almacenar el valor del Potenciometro
float valorPotenciometro=0;
float valorPotenciometro_N=0;

//Generamos una variable para el duty cycle del PWM
int dutyPWM=0;

//Caracteristicas del PWM, definicion
const int freq=5000;
const int canal=0; 
//De esta manera sera de 8 bits de 0 a 255
const int resolucion=8; 

//Almacenamos los datos de UART en un string
String dato;
String dato_aux;
boolean EscrituraCompletada=false;
String tiempo_X;
String duty;

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
  //Atenuamos la señal porque nuestra entrada es de 3.3V
  analogSetPinAttenuation(Epotenciometro, ADC_11db);

  //TIMER
   timer=timerBegin(0,80,true); // Preescalado para pasar a 1MHz
   timerAttachInterrupt(timer,&onTimer, true);
   //timerAlarmWrite(timer,10000000,true); //1millon/s, colocamos 10millones equivalente a 10s, true para que recargar automaticamente
   //timerAlarmEnable(timer);

  //Caracterizamos el PWM y asociamos el canal al puerto del LED
  ledcSetup(LedPin,freq,resolucion);
  ledcAttachPin(LedPin,canal);
}
//Recogida de dato de forma "asincrona" no es una funcion asincrona, como una interrupcion. 
//Nuevo dato en el buffer serial, se ejecutara el serialEvent.
void serialEvent()
{
  while (Serial.available())
  {
    char entradaByte= Serial.read();
    dato+=entradaByte; // Agregamos el byte de la entrada al string dato
      if(entradaByte == '\n')
      {
        //Se quita el salto de la linea final
        dato.trim();
        //Guardamos el dato en una variable auxiliar antes de borrarlo
        dato_aux=dato;
        //Borramos el dato para volver a rellenarlo
        dato= "";

        //La escritura se ha completado
        EscrituraCompletada=true;

      }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

 if (EscrituraCompletada)
 {
   if(dato_aux.equals("ADC"))
   {
     //Agregamos el valor analogico medido por el potenciometro en una variable de tipo float
     valorPotenciometro= analogRead(Epotenciometro);
     //Hay que escalarla a 3.3V porque el valor que nos dara sera de 0 a 4095
     valorPotenciometro_N= valorPotenciometro* (3.3/4095); 
     //Sacamos por pantalla el valor
     Serial.print ("Valor del ADC: ");
     Serial.println(valorPotenciometro_N);
     EscrituraCompletada=false;
   }
  //Chequeamos que en el string tengamos ADC( , de esta manera sabremos diferenciar del ADC normal
  //para poder modificar el timer
  else if (dato_aux.indexOf("ADC(")==0)
  {
    //Nos quedamos con valor de la posicion 4 del string
      tiempo_X= dato_aux.substring(4,6);
    //Pasamos este valor a entero
    X_Entero=tiempo_X.toInt();
    X_seg=X_Entero*1000000;
    if (X_Entero != 0)
    {
      timerAlarmWrite(timer,X_seg,true);
      timerAlarmEnable(timer);
      if (ContadorInterrupcion > 0)
        {
          portENTER_CRITICAL(&timerMux);
          ContadorInterrupcion--;
          portEXIT_CRITICAL(&timerMux);

          ContadorInterrupcion++;

          //Leemos de nuevo el valor y lo muestreamos por pantalla
          valorPotenciometro=analogRead(Epotenciometro);
          valorPotenciometro_N= valorPotenciometro* (3.3/4095); 
          Serial.print("Valor ADC: ");
          Serial.println(valorPotenciometro_N);
          Serial.println("Valor del Temporizador: ");
          Serial.println(X_seg);

        }

    }

  }
  else if (dato_aux.indexOf("PWM(")==0)
  {
    //Nos quedamos con el dato despues del parentesis
    duty=dato_aux.substring(4,6);
    //Pasamos el dato a tipo INT y luego lo almacenamos en la variable dutyPWM 
    dutyPWM=duty.toInt();
    //Escalamos el valor de entrada por consola de 0 a 9 -> 0 a 255
    dutyPWM= map (dutyPWM,0,9,255,0);
    //Le metemos el duty programado para el PWM para el led
    ledcWrite(canal,dutyPWM);

  }

 }
}
