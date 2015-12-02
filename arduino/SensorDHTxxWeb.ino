// InternetDeLasCosas.cl
// Script que obtiene la temperatura y humedad desde un sensor DHTxx
// y la envia a un servidor web
//
// Escrito por @joniuz

#include <SPI.h>
#include <Ethernet.h>
// Libreria para Sensores DHT
#include "DHT.h"

#define DHTPIN 2     // Pin del Arduino al cual esta conectado el pin 2 del sensor

// Descomentar segun el tipo de sensor DHT usado
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Inicializa el sensor DHTxx
DHT dht(DHTPIN, DHTTYPE);

// Configuracion de fotocelda
int fotocelda = 0;
int luminosidad;

// Configuracion Ethernet Shield
// Mac unica de cada EthernetShield - Configurarla para la shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };

// Direccion IP Fija en caso de que no pueda obtenerla por DHCP
IPAddress ip(172,17,17,217);

// Direccion del DSN en caso de que no pueda obtenerla por DHCP
//IPAddress myDns(172,17,17,1);

// Inicializa la instancia client
EthernetClient client;

// Direccion del servidor - Cmabiar por la direccion IP local o nombre
char server[] = "dcs.iot.cl";

// Ultima conexion al servidor medida en millisegundos
unsigned long ultimaConexion = 0;          
              
// Intervalo en milisegundos entre conexiones
const unsigned long intervaloConexion = 20000;   

// Leds conectados a Salidas Digitales
int led_1;
int led_2;
int led_3;

void setup() {
  
  // Configura salidas digitales para los Leds
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  // Inicializa puerto serial
  Serial.begin(9600);
  Serial.println("######### Internetdelascosas.cl #########");
  Serial.print("Sensor de temperatura-humedad DHT");
  Serial.print(DHTTYPE);
  Serial.println(" / web");
  Serial.println();
  
  // Espera 1 segundo para que se inicie la tarjeta Ethernet
  delay(1000);
  
  // Intenta inicializar la EthernetShield via DCHCP
  Serial.println("Iniciando EthernetShield via DHCP");
  if(!Ethernet.begin(mac)) {
    // Si el DHCP falla, inicia la EthernetShield con IP estatica
    Serial.println("Falla al obtener IP desde DHCP, iniciando con IP estatica");
    Ethernet.begin(mac, ip);
  }
  
  // Imprime la direccion IP de la tarjeta
  Serial.print("Direccion IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println();
  
  // Inicializa Sensor DHTxx
  Serial.print("Iniciando Sensor DHT");
  Serial.println(DHTTYPE);
  Serial.println();
  dht.begin();
  // Espera dos segundos para dar tiempo al sensor mientras obtiene muestra
  delay(2000);
}
// Loop principal
void loop() {
  // Si hay conexion con el servidor
  if (client.connected()) {
    if (client.available()) {
      // Obtiene la respuesta del servidor y busca los tags xml <led_1> <led_2> y <led_3>
      if(client.find("<led_1>")) {
        led_1 = client.parseInt();
      }
      if(client.find("<led_2>")) {
        led_2 = client.parseInt();
      }
      if(client.find("<led_3>")) {
        led_3 = client.parseInt();
      }
      // Imprime valores enviados por servidor
      Serial.println("Valores enviados por servidor");
      Serial.print("Led 1: ");
      Serial.println(led_1);
      Serial.print("Led 2: ");
      Serial.println(led_2);
      Serial.print("Led 3: ");
      Serial.println(led_3);
      // Cierra conexion
      Serial.println("Cerrando conexion..."); 
      Serial.println();
      client.stop();
      // Enciende Leds segun valores
      if (led_1)
        digitalWrite(5, HIGH);
      else
        digitalWrite(5, LOW);
      if (led_2)
        digitalWrite(6, HIGH);
      else
        digitalWrite(6, LOW);
      if (led_3)
        digitalWrite(7, HIGH);
      else
        digitalWrite(7, LOW);
    }
  }
  else if(millis() - ultimaConexion > intervaloConexion) {
    httpRequest();
  }
}
// Fin del loop principal

// Funcion que realiza la conexion http al servidor y obtiene la respuesta
void httpRequest() {
  // Obtiene los datos del sensor DHTxx
  // Obtiene la Humedad
  float h = dht.readHumidity();
  // Obtiene la Temperatura en Celsius
  float t = dht.readTemperature();
  
  // Control de errores, valida que se obtuvieron valores para los datos medidos
  if (isnan(h) || isnan(t)) {
    Serial.println("Falla al leer el sensor DHTxx!");
    return;
  }
  
  Serial.print("Temperatura: ");
  Serial.println(t);
  Serial.print("Humedad: ");
  Serial.println(h);
  
  // Obtiene datos de la fotocelda
  luminosidad = analogRead(fotocelda);  
  
  Serial.print("Luminosidad = ");
  Serial.print(luminosidad);
  Serial.println();
  // Se conecta al servidor en el puerto 80 (web)
  if (client.connect(server, 80)) {
    // Envia el dato al puerto serial
    Serial.println("Iniciando conexion...");
    // Envia el requerimiento via GET
    client.print("GET /sensorarduino.php?id=joniuz&temperatura=");
    client.print(t);
    client.print("&humedad=");
    client.print(h);
    client.print("&luminosidad=");
    client.print(luminosidad);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: Arduino-Ethernet");
    client.println("Connection: close");
    client.println();

    // Actualiza el tiempo en milisegundos de la ultima conexion
    ultimaConexion = millis();
  } 
}

