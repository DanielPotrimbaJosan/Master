int Epotenciometro=34; //Entrada del Potenciometro
int valor=0;
volatile int ContadorInterrupcion; // durante la compilacion optimizada no remueve esta variable
int ContadorInterrupcionTotal;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux =portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer()
{
portENTER_CRITICAL_ISR(&timerMux);
ContadorInterrupcion++;
portEXIT_CRITICAL_ISR(&timerMux);
}

   void setup() {
      // put your setup code here, to run once:
        Serial.begin(115200);
        timer=timerBegin(0,80,true); // Preescalado para pasar a 1MHz
        timerAttachInterrupt(timer,&onTimer, true);
        timerAlarmWrite(timer,10000000,true); //1millon/s, colocamos 10millones equivalente a 10s, true para que recargar automaticamente
        timerAlarmEnable(timer);

    }

    void loop() {
      // put your main code here, to run repeatedly:
        if (ContadorInterrupcion > 0)
        {
          portENTER_CRITICAL (&timerMux);
          ContadorInterrupcion--;
          portEXIT_CRITICAL(&timerMux);

          ContadorInterrupcionTotal++;
    
  
  Serial.print ("Valor del Potenciometro: "); //sacar por consola el texto
  Serial.println (valor); //sacar por consola la variable "valor"
  }
 
delay(100);
    }
