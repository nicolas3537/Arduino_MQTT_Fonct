
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

#define debug 0

// On créé l'objet pour la sonde DHT11
DHT dht(DHT11PIN, DHT11);

// Variables pour comparer l'ancienne valeur des sondes à la nouvelle
int t_old = 0;
int h_old = 0;
char h_str[10];
char t_str[10];

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
    client.publish("homeassistant/sensor/Baie/Temperature","0");
    client.publish("homeassistant/sensor/Baie/Hygrometrie","0");
    client.publish("homeassistant/sensor/Baie/Ventilateur1","0");
    client.publish("homeassistant/sensor/Baie/Ventilateur2","0");
    client.publish("homeassistant/sensor/Baie/Reboot","1");
  }
  else {
      if (debug) Serial.print("echec, code erreur= ");
      if (debug) Serial.println(client.state());
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
  if (debug) Serial.println(IP_Arduino);

  Serial.println(F("*** Fin de la configuration ***"));
}

void loop()
{
client.loop();
unsigned long currentMillis = millis();
    
// S'il s'est écoulé 30 secondes,
if ( currentMillis - previousMillis >= 30000 ) 
  {
    previousMillis = currentMillis;

    int h = dht.readHumidity();
    int t = dht.readTemperature();
    itoa(h,h_str,10);
    itoa(t,t_str,10);
    client.publish("homeassistant/sensor/Baie/Temperature",t_str);
    client.publish("homeassistant/sensor/Baie/Hygrometrie",h_str);
    if (debug) {
    Serial.print(F("T11 "));
    Serial.print(t);
    Serial.print(F("C - H11 "));
    Serial.print(h);
    Serial.print(F("%"));
    Serial.print(F("C || Millis : "));
    Serial.print(millis()/1000);
    Serial.print(F(" || NbPbDeco : "));
    Serial.print(NombreProblemeDeconnexion);
    Serial.print(F(" || NbErReseau : "));
    Serial.println(NombreErreurReseau);
    }

    if((float)t>= 28.00){
      digitalWrite(Relais_1, HIGH);
      if (debug) Serial.println("Relais 1 allimenté");
      client.publish("homeassistant/sensor/Baie/Ventilateur1","1");
      etat_ventil_1= 1;
      if((float)t>=32.00){
        digitalWrite(Relais_2, HIGH);
        if (debug) Serial.println("Relais 2 allimenté"); 
        client.publish("homeassistant/sensor/Baie/Ventilateur2","1");
        etat_ventil_2= 1;
      }
    }
    else {
      digitalWrite(Relais_1, LOW);
      digitalWrite(Relais_2, LOW);
      if (debug) Serial.println("Les relais sont éteint");
      client.publish("homeassistant/sensor/Baie/Ventilateur1","0");
      client.publish("homeassistant/sensor/Baie/Ventilateur2","0");
      etat_ventil_1= 0;
      etat_ventil_2= 0;
    }

    if(etat_ventil_1 != etat_ventil_1_old){
      etat_ventil_1_old = etat_ventil_1;
      if (debug) Serial.println("Trame ventilateur 1 envoyé");                
    }

    if(etat_ventil_2 != etat_ventil_2_old){
      etat_ventil_2_old = etat_ventil_2;
      if (debug) Serial.println("Trame ventilateur 2 envoyé");                  
    }
    if (debug) {
    Serial.print(etat_ventil_1);
    Serial.print(etat_ventil_1_old);
    Serial.print(etat_ventil_2);
    Serial.print(etat_ventil_2_old);  
    }

    if(t_old != t)
    {
      // POUR DEBUG
      unsigned long DebutEnvoi = millis();
      
      // lance la fonction pour envoyer en GET les infos
          t_old = t ;
      
      // POUR DEBUG
      if (debug) {
      unsigned long FinEnvoi = millis();
      Serial.print(F("Debut envoi : "));
      Serial.print(DebutEnvoi);
      Serial.print(F(" - Fin envoi : "));
      Serial.print(FinEnvoi);
      Serial.print(F(" - Duree totale : "));
      int DureeEnvoi = FinEnvoi - DebutEnvoi;
      Serial.println(DureeEnvoi);
      }
    }
  }
}