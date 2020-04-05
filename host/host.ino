#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SparkFun_Ublox_Arduino_Library.h>

//CONSTANTS
SFE_UBLOX_GPS myGPS;
long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.
int id = 0;
char payload[295];

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//CONFIGURE WEBSOCKET
void handleRoot(AsyncWebServerRequest * request) {
  request->send(SPIFFS, "/index.html", "text/html");
}
void onWSEvent(AsyncWebSocket * server,
               AsyncWebSocketClient * client,
               AwsEventType type,
               void * arg,
               uint8_t * data,
               size_t length) {
  switch (type) {
    case WS_EVT_CONNECT:
      {
        Serial.println("WS client connect");
      }
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("WS client disconnect");
      break;
    case WS_EVT_DATA:
      {

      }
      break;
  }
}

void setup() {
  Serial.begin(115200);

  const char * ssid = "";
  const char * key = "";

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File configFile = SPIFFS.open("/config.json", "r");
  DynamicJsonDocument doc(1024);
  if (!configFile) {
    Serial.println("Failed to open config.json for reading");
    return;
  } else {
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      Serial.print("Error parsing config.json [");
      Serial.print(error.c_str());
      Serial.println("]");
    }

    ssid = doc["ssid"];
    key = doc["key"];

    configFile.close();
  }

  //Serial.print("ssid: '");
  //Serial.print(ssid);
  //Serial.println("'");
  //Serial.print("key: '");
  //Serial.print(key);
  //Serial.println("'");
  WiFi.begin(ssid, key);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up mDNS responder");
  } else {
    Serial.println("mDNS responder started: esp32.local");
  }

  Wire.begin();

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR

  ws.onEvent(onWSEvent);
  server.addHandler( & ws);
  server.on("/", HTTP_GET, handleRoot);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load favicon.ico file
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/ico");
  });

  // Route to load script.js file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "application/javascript");
  });

  // Route to load leaflet.css file
  server.on("/leaflet.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/leaflet.css", "text/css");
  });

  // Route to load leaflet-src.js file
  server.on("/leaflet-src.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/leaflet-src.js", "application/javascript");
  });

  // Route to load leaflet-realtime.min.js file
  server.on("/leaflet-realtime.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/leaflet-realtime.min.js", "application/javascript");
  });

  server.on("/json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "application/json", payload);
  });

  server.begin();

  MDNS.addService("http", "tcp", 80);

}

void loop() {

  if (millis() - lastTime > 1000)
  {
    float latitude = myGPS.getLatitude();
    latitude = latitude / 10000000;

    float longitude = myGPS.getLongitude();
    longitude = longitude / 10000000;


    id++;
    if (id >= 200) {
      (id = 0);
    }

    lastTime = millis(); //Update the timer

    const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);

    doc["type"] = "Feature";

    JsonObject properties = doc.createNestedObject("properties");
    properties["id"] = (id);
    properties["color"] = "red";
    properties["name"] = "SAM-M8Q";
    properties["popupContent"] = "Current location";

    JsonObject geometry = doc.createNestedObject("geometry");
    geometry["type"] = "Point";

    JsonArray geometry_coordinates = geometry.createNestedArray("coordinates");
    geometry_coordinates.add(longitude);
    geometry_coordinates.add(latitude);

    //serializeJson(doc, Serial);
    //Serial.println();
    char json_string[295];
    serializeJson(doc, json_string);
    char payload[295];
    snprintf_P(payload, sizeof(payload), json_string);
    ws.textAll(payload);
  }
}
