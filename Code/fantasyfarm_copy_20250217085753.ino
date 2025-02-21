// Me cago

#include "config.h"
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define SENHUM A0
#define RELE_AGUA 14
#define RELE_FERT 12

// set up the 'digital' feed 

AdafruitIO_Feed *temperaturaFeed = io.feed("temperatura");
AdafruitIO_Feed *humedadFeed = io.feed("humedad");
AdafruitIO_Feed *humedadSueloFeed = io.feed("humedadtierra");
AdafruitIO_Feed *bombaaguaFeed = io.feed("bombaagua");
AdafruitIO_Feed *bombafertFeed = io.feed("bombafert");

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs. 

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  // start the serial connection
  Serial.begin(115200);
  dht.begin();
  
  pinMode(RELE_AGUA, OUTPUT);
  pinMode(RELE_FERT, OUTPUT);
  // wait for serial monitor to open
  while(! Serial);
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
// set up a message handler for the 'relay' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io
  bombaaguaFeed->onMessage(handleMessage);
  bombafertFeed->onMessage(handleMessage1);
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    yield();
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  
  // Get the last known value from the feed
  bombaaguaFeed->get();
  bombafertFeed->get();
  Serial.println("Bombas guardadas.");
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = analogRead(SENHUM);
  
  // Conversión lectura analogica en porcentaje. 
  f = map(f, 1023, 330, 0, 100);
  
  // save the current state to the 'digital' feed on adafruit io
  Serial.print(F("Humedad ambiental: "));
  Serial.println(h);
  Serial.print(F("Temperatura: "));
  Serial.println(t);
  Serial.print(F("Humedad suelo: "));
  Serial.println(f);
  Serial.println("--------------------------");

  humedadFeed->save(h);
  temperaturaFeed->save(t);
  humedadSueloFeed->save(f);
  yield();
  delay(10000);
}

void handleMessage(AdafruitIO_Data *data) {

  Serial.print("Mensaje recibido en bomba de agua. ");
  Serial.println(data->toChar());

// Check to see if the morning scheduled trigger has executed 
  if (strcmp(data->toChar(), "1") == 0) {
      Serial.println("Encendiendo bomba de agua. ");
      digitalWrite(RELE_AGUA, LOW);
  }
// Check to see if the evening scheduled action has executed
  else if (strcmp(data->toChar(), "0") == 0) {
      Serial.println("Apagando bomba de agua. ");
      digitalWrite(RELE_AGUA, HIGH);
  }
  else {
      Serial.println("Datos inesperados en bomba de agua. ");
  }
}

void handleMessage1(AdafruitIO_Data *data) {

  Serial.print("Mensaje recibido en bomba de fertilizante. ");
  Serial.println(data->toChar());

// Check to see if the morning scheduled trigger has executed
  if (strcmp(data->toChar(), "1") == 0) {
      Serial.println("Activando bomba fertilizante. ");
      digitalWrite(RELE_FERT, LOW);
  }
// Check to see if the evening scheduled action has executed 
  else if (strcmp(data->toChar(), "0") == 0) {
      Serial.println("Desactivando bomba fertilizante. ");
      digitalWrite(RELE_FERT, HIGH);
  }
  else {
      Serial.println("Datos inesperados en bomba fertilizante. ");
  }
}
