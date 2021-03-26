
/*
 Basic MQTT example with Authentication

  - connects to an MQTT server, providing username
    and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT_U.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 159);
IPAddress server(192, 168, 0, 23);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

//On définie les pins
#define DHT11PIN 2     // PIN pour la sonde DHT11
#define Relais_1 4
#define Relais_2 5

// On créé l'objet pour la sonde DHT11
DHT dht(DHT11PIN, DHT11);

// Variables pour comparer l'ancienne valeur des sondes à la nouvelle
int t_old = 0;
int h_old = 0;

// POUR DEBUG
// Variable pour compter le nombre de connexion échouée de client.connect
int NombreErreurReseau = 0 ;
int NombreErreurReseau_old = -1;
int NombreProblemeDeconnexion = 0 ;
boolean etat_ventil_1 = 0;
boolean etat_ventil_2 = 0;
boolean etat_ventil_1_old = 0;
boolean etat_ventil_2_old = 0;

// Variable de Tempo pour déclenchement de lecture
unsigned long previousMillis = 0;

//MQTT définition
#define Clientarduino "Clientarduino"
#define MQTT_USERNAME "homeassistant"
#define MQTT_KEY "ohs8Phookeod6chae0ENg5aingeite8Jaebooziheevug0huinei8Ood9iePoh9l"



void setup()
{
  Ethernet.begin(mac, ip);
  // Note - the default maximum packet size is 128 bytes. If the
  // combined length of clientId, username and password exceed this use the
  // following to increase the buffer size:
  // client.setBufferSize(255);
   
  if (client.connect(Clientarduino, MQTT_USERNAME, MQTT_KEY)) {
    client.publish("outTopic","hello world");
    client.subscribe("inTopic");}
  else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
  }

  // On initialise la sonde DHT11
  dht.begin();

  //Définition des sortie pour les relais
  pinMode(Relais_1, OUTPUT);
  pinMode(Relais_2, OUTPUT);

  // On ouvre le port série pour DEBUG
  Serial.begin(9600);

  //Pour DEBUG
  //Obtenir l'adresse IP de l'arduino
  IPAddress IP_Arduino = Ethernet.localIP();
  Serial.println(IP_Arduino);

  Serial.println(F("*** Fin de la configuration ***"));
}

void loop()
{
  client.loop();
}