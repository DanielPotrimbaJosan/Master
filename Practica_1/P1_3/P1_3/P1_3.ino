int Epotenciometro=34; //Entrada del Potenciometro
int valor=0;
int valor_nuevo=0; //Valor escalado del ADC
const int LedPin= 5; //GPIO 5 salida del PWM

   void setup() {
      // put your setup code here, to run once:
        Serial.begin(115200);
        pinMode(LedPin, OUTPUT);

    }

    void loop() {
      // put your main code here, to run repeatedly:
         valor=analogRead(Epotenciometro);  //Valor del Potenciometro 
         valor_nuevo= map(valor,0,4095,0,255);
        Serial.print ("Valor del Potenciometro: ");
        Serial.println (valor);
        analogWrite(LedPin, valor_nuevo);
        delay(1000);
  }
 
