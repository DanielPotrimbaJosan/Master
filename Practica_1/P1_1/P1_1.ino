int Epotenciometro=34; //Entrada del Potenciometro
int valor=0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  valor=analogRead(Epotenciometro); //el dato leido lo guardamos en la variable global valor
  Serial.print ("Valor del Potenciometro: "); //escribir por consola
  Serial.println (valor); //escribir por consola el valor 

delay(1000);
}
