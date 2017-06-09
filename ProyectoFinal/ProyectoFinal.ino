#include <LiquidCrystal.h>
#include <stdlib.h>
#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>
#include <FuzzyRule.h>

Fuzzy* fuzzy=new Fuzzy(); //Se inicializa el fuzificador.

LiquidCrystal lcd(12,11,5,4,3,2);
int ventil=13;

void setup() {
  // put your setup code here, to run once:

  lcd.begin(16,2);

  Serial.begin(9600); //Se inicia la comunicación serial Baudios para comunicación con PC
  //dht.begin(); //Se inicia el sensor

  //Espacio Para Iniciar los pines


  FuzzyInput* Pasajeros= new FuzzyInput(1); //Se crea la variable de entrada "Pasajeros"


  FuzzySet* Pocos= new FuzzySet(0,0,10,14); // Se crea la función para la etiqueta linguistica "Pocos"
  Pasajeros->addFuzzySet(Pocos);            // Se añade la etiqueta al conjunto Pasajeros
  FuzzySet* Medio= new FuzzySet(12,16,18,22); // Se crea la función para la etiqueta linguistica "Medio"
  Pasajeros->addFuzzySet(Medio);            // Se añade la etiqueta al conjunto Pasajeros
  FuzzySet* Muchos= new FuzzySet(20,24,30,30); // Se crea la función para la etiqueta linguistica "Muchos"
  Pasajeros->addFuzzySet(Muchos);          // Se añade la etiqueta al conjunto Pasajeros

  fuzzy->addFuzzyInput(Pasajeros);        // Se añade al fuzificador el  conjunto Pasajeros.


  FuzzyInput* Temperatura= new FuzzyInput(2);  // Se crea la variable de entrada "Temperatura".

  FuzzySet* Ideal= new FuzzySet(20,20,22,25); // Se crea la función para la etiqueta linguistica "Ideal"
  Temperatura->addFuzzySet(Ideal);             // Se añade la etiqueta al conjunto Temperatura
  FuzzySet* Caliente= new FuzzySet(22,23,25,27);  // Se crea la función para la etiqueta linguistica "Caliente"
  Temperatura->addFuzzySet(Caliente);         // Se añade la etiqueta al conjunto Temperatura
  FuzzySet* MuyCaliente= new FuzzySet(25,29,32,32); // Se crea la función para la etiqueta linguistica "MuyCaliente"
  Temperatura->addFuzzySet(MuyCaliente);      // Se añade la etiqueta al conjunto Temperatura

  fuzzy->addFuzzyInput(Temperatura);      // Se añade al fuzificador el  conjunto Temperatura.


  FuzzyOutput* AireAcondicionado= new FuzzyOutput(1); //Se crea el conjunto de salida "Air Acondicionado"

  FuzzySet* Baja= new FuzzySet(15,17,17,19);  // Se crea la función para la etiqueta linguistica "Baja"
  AireAcondicionado->addFuzzySet(Baja);       // Se añade la etiqueta al conjunto Aire Acondicionado
  FuzzySet* Media= new FuzzySet(18,20,20,22); // Se crea la función para la etiqueta linguistica "Media"
  AireAcondicionado->addFuzzySet(Media);      // Se añade la etiqueta al conjunto Aire Acondicionado
  FuzzySet* Alta= new FuzzySet(20,23,23,25);  // Se crea la función para la etiqueta linguistica "Alta"
  AireAcondicionado->addFuzzySet(Alta);       // Se añade la etiqueta al conjunto Aire Acondicionado
  
  fuzzy->addFuzzyOutput(AireAcondicionado);   // Se añade al fuzificador el conjunto de salida "Aire Acondicionado".


  //#-----Regla 1 -----#

  FuzzyRuleAntecedent* CantidadPasajerosMuchoORMedio = new FuzzyRuleAntecedent();   //Se crea una nueva regla
  CantidadPasajerosMuchoORMedio->joinWithOR(Muchos,Medio);                          // Se hace la unión entre conjuntos dependiendo de la regla

  FuzzyRuleAntecedent* TemperaturaMuyCaliente = new FuzzyRuleAntecedent();          
  TemperaturaMuyCaliente->joinSingle(MuyCaliente);

  FuzzyRuleAntecedent* IfPasajerosMuchoOrMedioANDTemperaturaMuyCaliente= new FuzzyRuleAntecedent();
  IfPasajerosMuchoOrMedioANDTemperaturaMuyCaliente->joinWithAND(CantidadPasajerosMuchoORMedio,TemperaturaMuyCaliente);

  FuzzyRuleConsequent* AireBajo= new FuzzyRuleConsequent();                         // Se crea la regla de salida correspondiente a lo evaluado anteriormente.
  AireBajo->addOutput(Baja);                                                        // Se agrega al conjunto de salida.
  
  FuzzyRule* fuzzyRule1= new FuzzyRule(1,IfPasajerosMuchoOrMedioANDTemperaturaMuyCaliente, AireBajo);   // Se agrega el valor de la regla al conjunto de salida.
  fuzzy->addFuzzyRule(fuzzyRule1);                                                                      // Se agrega la nueva regla fuzificada al motor fuzificador.


  //#-----Regla 2 ------#

  FuzzyRuleAntecedent* TemperaturaCaliente = new FuzzyRuleAntecedent();
  TemperaturaCaliente->joinSingle(Caliente);

  FuzzyRuleConsequent* AireMedia = new FuzzyRuleConsequent();
  AireMedia->addOutput(Media);

  FuzzyRule* fuzzyRule2= new FuzzyRule(2,TemperaturaCaliente,AireMedia);
  fuzzy->addFuzzyRule(fuzzyRule2);


  //#-----Regla 3 ------#

  FuzzyRuleAntecedent* PasajerosMuchosANDTemperaturaIdeal= new FuzzyRuleAntecedent();
  PasajerosMuchosANDTemperaturaIdeal->joinWithAND(Muchos,Ideal);

  FuzzyRule* fuzzyRule3= new FuzzyRule(3,PasajerosMuchosANDTemperaturaIdeal,AireMedia);
  fuzzy->addFuzzyRule(fuzzyRule3);
  

  //#------Regla 4------#

  FuzzyRuleAntecedent* PasajerosPocosORMedio= new FuzzyRuleAntecedent();
  PasajerosPocosORMedio->joinWithOR(Pocos,Medio);

  FuzzyRuleAntecedent* IfPasajerosPocosORMedioANDTemperaturaIdeal= new FuzzyRuleAntecedent();
  IfPasajerosPocosORMedioANDTemperaturaIdeal->joinWithAND(PasajerosPocosORMedio,Ideal);

  FuzzyRuleConsequent* AireAlta= new FuzzyRuleConsequent();
  AireAlta->addOutput(Alta);

  FuzzyRule* fuzzyRule4= new FuzzyRule(4,IfPasajerosPocosORMedioANDTemperaturaIdeal,AireAlta);
  fuzzy->addFuzzyRule(fuzzyRule4);

  //#--------Regla 5--------#
  FuzzyRuleAntecedent* PasajerosPocosANDTempMuyCaliente= new FuzzyRuleAntecedent();
  PasajerosPocosANDTempMuyCaliente->joinWithAND(Pocos,MuyCaliente);

  FuzzyRule* fuzzyRule5= new FuzzyRule(5,PasajerosPocosANDTempMuyCaliente,AireMedia);
  fuzzy->addFuzzyRule(fuzzyRule5);   

}

void loop() {
  // put your main code here, to run repeatedly:

  randomSeed(analogRead(0));            // Se generan números aleatorios  
  int pasajeros= random(0,30);          // del 0 al 30 (cantidad de pasajeros)

  //Capturando lectura del sensor LM35
  int value = analogRead(A0);           // Se lee la señal del sensor
  float celsius = (value*500.0) /1023;  // Se convierte a grados celsius
  fuzzy->setInput(1,pasajeros);         // Se llama el fuzificador con la entrada de pasajeros.
  fuzzy->setInput(2,celsius);           // Se llama el fuzificador con el valor hallado para la temperatura
  fuzzy->fuzzify();                     // Se fuzifican los valorees de entrada.
  float aire=fuzzy->defuzzify(1);       // Se desfizifican los valorees de entrada.
  float ventilador=map(aire,15,25,255,0); // Se mapea la temperatura deseada en relación con la velocidad de ventilación del ventilador.
  analogWrite(ventil,ventilador);         // Se manda el voltaje al ventilador.


  lcd.setCursor(0,0);
  lcd.print("P:");
  lcd.print(pasajeros);
  lcd.print(" ");
  lcd.setCursor(6,0);
  lcd.print("T:");
  lcd.print(celsius);
  lcd.print("C");
  lcd.print((char)223);
  lcd.setCursor(0,1);
  lcd.print("T.Ambiente:");
  lcd.print(aire);
  lcd.setCursor(0,0);
  Serial.println(pasajeros);
  Serial.print(celsius);
  Serial.println(" C° ");
  Serial.println(aire);
  delay(3000);
}
