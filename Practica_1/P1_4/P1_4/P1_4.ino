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

boolean FinTransmision=false;
//Para almacenar los datos de UART en un string
String tiempo_X;
String duty;

String CaracteresEntrada= "";
String CaracteresEntradaAux;

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

void loop() 
{
  // put your main code here, to run repeatedly:
  if (FinTransmision)
  {
    
      //Comparamos el String de la consola en este caso con lo que hay en ""
   if(CaracteresEntradaAux.equals("ADC"))
      {
        //Agregamos el valor analogico medido por el potenciometro en una variable de tipo float
        valorPotenciometro= analogRead(Epotenciometro);
        //Hay que escalarla a 3.3V porque el valor que nos dara sera de 0 a 4095
        valorPotenciometro_N= valorPotenciometro* (3.3/4095); 
        //Sacamos por pantalla el valor
        Serial.print ("Valor del ADC: ");
        Serial.println(valorPotenciometro_N);
        FinTransmision=false; // PARA PARAR LA TRANSMISION
      }
  
      //Si el texto escrito en la consola empieza con ADC( entonces cogeremos la siguiente posicion para el tiempo
      // y que se acabe con la ) cogeremos el valor de dentro
    else if (CaracteresEntradaAux.startsWith("ADC(") && CaracteresEntradaAux.endsWith(")") ) 
      {
        tiempo_X= CaracteresEntradaAux.substring(4,6);
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

                  //ContadorInterrupcion++;

                  //Leemos de nuevo el valor y lo muestreamos por pantalla
                  valorPotenciometro=analogRead(Epotenciometro);
                  valorPotenciometro_N= valorPotenciometro* (3.3/4095); 
                  Serial.print("Valor ADC: ");
                  Serial.println(valorPotenciometro_N);
                  Serial.print("Valor del Temporizador: ");
                  Serial.println(X_Entero);
                }
            }
       }
  
  /*else if (CaracteresEntrada.indexOf("PWM(")==0)
  {
    //Nos quedamos con el dato despues del parentesis
    duty=CaracteresEntrada.substring(4,6);
    //Pasamos el dato a tipo INT y luego lo almacenamos en la variable dutyPWM 
    dutyPWM=duty.toInt();
    //Escalamos el valor de entrada por consola de 0 a 9 -> 0 a 255
    dutyPWM= map (dutyPWM,0,9,255,0);
    //Le metemos el duty programado para el PWM para el led
    ledcWrite(canal,dutyPWM);

  }*/
  
  }      
}
//La funcion te permite recibir datos de una forma asincrona, no es asincrona
// Se ejecuta en cada iteracion de la funcion loop 
//Se ejecutara si hay algun dato en el buffer del puerto serial (en este caso la consola)

void serialEvent()
{

  //Con este comando podemos saber si existe algun dato 
  while (Serial.available())
  {
    //leemos el dato recibido de 1 byte
    char LecturaConsola= Serial.read();
    CaracteresEntrada+=LecturaConsola; // Se agrega los caracteres leidos a la variable de tipo string
      //Si el dato recibido es un fin de linea, mediante \n sabremos que ya se termino la transmision
      if(LecturaConsola == '\n') 
      {
        //La escritura se ha completado
        FinTransmision=true;
        
        //Se quita el salto de la linea final sino no funciona
        CaracteresEntrada.trim();
        //Guardamos el dato en una variable auxiliar antes de borrarlo
        CaracteresEntradaAux= CaracteresEntrada;
        //Borramos el dato para volver a rellenarlo
        CaracteresEntrada= "";
      }
  }
}
