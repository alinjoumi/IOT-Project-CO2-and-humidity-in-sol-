// ******************************************** ****
// Mesure analogique et numérique de l’humidité du sol
// Avec le module Humidité Sol
// Mesure analogique et numérique de co2
//Envoi Vers Ubidots
// ************************* ******************************
// connexion Wifi
#include <ESP8266WiFi.h>

// Ubidots Connexion
#include "Ubidots.h"
#define TOKEN "BBFF-O2KZiwAEY7Yhpi05sNPdJW1MPVz98a"

// definir les paramètres du connexion wifi
#define WIFISSID "Guest_ISETCOM"
#define PASSWORD "SUPCOM**1920$"



/// La résistance de charge sur la carte
#define RLOAD 10.0
/// Résistance d'étalonnage au niveau de CO2 atmosphérique
#define RZERO 76.63
/// Paramètres de calcul des ppm de CO2 à partir de la résistance du capteur
#define PARA 116.6020682
#define PARB 2.769034857
/// Niveau de CO2 atmosphérique à des fins d'étalonnage
#define ATMOCO2 397.13
#include "MQ135.h"                                                                        
uint8_t mq135Pin = A0;     // Pin sur lequel est branché de MQ135
MQ135 gasSensor = MQ135(mq135Pin);  // Initialiser l'objet MQ135 sur le Pin spécifié

Ubidots client(TOKEN); // Créer le client Ubidots

// Assigner les variables aux pins correspendants
int R_D1 = D1;
int R_D2 = D2;
int R_D3 = D3;



void setup()
{
  Serial.begin(115200);// Initialiser le port série à 1152000 bps 
  // Intitialiser les inputs/outputs 
  pinMode(mq135Pin, INPUT);
  pinMode(A0,INPUT);// pin A0 en entrée analogique
  pinMode(R_D1,OUTPUT);
  pinMode(R_D2,OUTPUT);
  pinMode(R_D3,OUTPUT);
  
 
  WiFi.begin(WIFISSID, PASSWORD);
  client.setDebug(true);
   
}

// Fonctions pour controler les valeurs présent sur le pin A0
int analogRead1(){
  
  digitalWrite(R_D1,HIGH);
  digitalWrite(R_D2,LOW);
  digitalWrite(R_D3,LOW);
  delay(1000);
  return analogRead(A0);
  
}

int analogRead2(){
    
  digitalWrite(R_D1,LOW);
  digitalWrite(R_D2,HIGH);
  digitalWrite(R_D3,LOW);
  delay(1000);
  return analogRead(A0);
}

int analogRead3(){
  digitalWrite(R_D1,LOW);
  digitalWrite(R_D2,LOW);
  digitalWrite(R_D3,HIGH);
  delay(1000);
  return analogRead(mq135Pin);
}
  
void loop() {
  float moisture_percentage1,moisture_percentage2;

  // Capteur Humidité pour 20 cm de profondeur
  moisture_percentage1 = ( 100 - ( (analogRead1()/1024.00) * 100 ) );
  delay(1000);
  
  // Capteur Humidité pour 20 cm de profondeur
  moisture_percentage2 = ( 100 - ( (analogRead2()/1024.00) * 100 ) );
  delay(1000);

  //Afficher les valeurs 
  Serial.print("Moisture Percentage at 20cm= ");
  Serial.println(moisture_percentage1);
  delay(1000);

  //Envoie des données vers Ubidots
  client.add("Moisture Percentage at 20cm", moisture_percentage1);
  bool bufferSent = false;
  bufferSent = client.send();  //  Enverra des données à une étiquette d'appareil qui correspond à l'ID de l'appareil

  if (bufferSent) {
    // Faire quelque chose si les valeurs ont été envoyées correctement
    Serial.println("Values sent by the device");
  }

  //Afficher les valeurs  
  Serial.print("Moisture Percentage at 40cm= ");
  Serial.println(moisture_percentage2);
  delay(1000);

  client.add("Moisture Percentage at 40 cm", moisture_percentage2);
  bufferSent = client.send();  // Enverra des données à une étiquette d'appareil qui correspond à l'ID de l'appareil

  if (bufferSent) {
    // Faire quelque chose si les valeurs ont été envoyées correctement
    Serial.println("Values sent by the device");
  }
  //Capteur CO2 MQ135
  //Afficher les valeurs
  Serial.println("CO2 Mesure: ");
  Serial.print("A0: ");
  int val = analogRead3();
  Serial.print(val);
  float resistance = ((1023./(float)val)) * RLOAD;  
  float ppm = PARA * pow((resistance/RZERO), -PARB)/ 1000;
  Serial.print(" ppm CO2: ");
  Serial.println(ppm);
  Serial.print("\n");
  delay(1000);

 //Envoie des données vers Ubidots
  client.add("Co2 Mesure ", ppm);
  bufferSent = client.send();  // Enverra des données à une étiquette d'appareil qui correspond à l'ID de l'appareil
  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Values sent by the device");
  }
 delay(1000);// Actualise la mesure chaque seconde
}
