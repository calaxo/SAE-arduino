#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>


#define SS_PIN 5
#define RST_PIN 27
#define EEPROM_SIZE 64
const int output26 = 26;
const int output13 = 13;
const int output12 = 12;
int inter = 32;

typedef struct struct_message {
  int id;
  char donnee[32];
} struct_message;

struct_message myData;




char *apiEndpoint = "http://saegeii.axel-cal.fr/api/post";  
MFRC522 mfrc522(SS_PIN, RST_PIN);
bool wifidispo = false;
AsyncWebServer server(80);

const char HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Formulaire Wi-Fi</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        header {
            background-color: #333;
            color: #fff;
            text-align: center;
            padding: 10px;
        }

        section {
            margin: 20px;
        }

        form {
            display: grid;
            gap: 10px;
        }

        label {
            display: block;
            margin-bottom: 5px;
        }

        input {
            padding: 8px;
        }

        button {
            padding: 10px;
            background-color: #333;
            color: #fff;
            border: none;
            cursor: pointer;
        }
    </style>
</head>
<body>

    <header>
        <h1>Configuration Wi-Fi</h1>
    </header>

    <section>
        <h2>Wi-Fi</h2>
        <form id="NetworkForm">
            <label for="ssid">SSID:</label>
            <input type="text" id="ssid" name="ssid" required>

            <label for="password">Mot de passe:</label>
            <input type="password" id="password" name="password" required>


        <h2>Réseau Secondaire</h2>
      
            <label for="identifier">Identifiant:</label>
            <input type="text" id="identifier" name="identifier" required>

            <label for="secondaryPassword">Mot de passe:</label>
            <input type="password" id="secondaryPassword" name="secondaryPassword" required>

            <button type="button" onclick="submitForm('NetworkForm')">Valider</button>
        </form>
    </section>

    <script>
        function submitForm(formId) {
            const form = document.getElementById(formId);
            const formData = new FormData(form);
            const jsonData = {};

            formData.forEach((value, key) => {
                jsonData[key] = value;
            });

            const url = 'http://192.168.4.1/config';
            fetch(url, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(jsonData),
            })
            .then(response => response.text())
            .then(data => {
                console.log('Réponse de lAPI:', data);

            })
            .catch(error => {
                console.error('Erreur lors de lenvoi des données:', error);

            });
        }
    </script>

</body>
</html>



)rawliteral";
 

bool has_credentials() {
  return EEPROM.read(0) == 0x42;
}

void save_credentials(char *ssid, char *pass) {
  char buf[EEPROM_SIZE];
  sprintf(buf, "%c%s:%s", 0x42, ssid, pass);
  EEPROM.writeString(0, buf);
  EEPROM.commit();
}

void load_credentials(char *ssid, char *pass) {
  if (!has_credentials()) {
    return;
  }
  char buf[EEPROM_SIZE];
  EEPROM.readString(1, buf, EEPROM_SIZE - 1);
  int i = 0;
  while ( i < EEPROM_SIZE && *(buf + i) != ':') {
    *ssid = *(buf + i);
    ssid++;
    i++;
  }
  *ssid = '\0';
  if (i == EEPROM_SIZE) {
    return;
  }
  i++;

  while ( i < EEPROM_SIZE && *(buf + i) != '\0') {
    *pass = *(buf + i);
    pass++;
    i++;
  }
  *pass = '\0';
}

void erase_credentials() {
  EEPROM.write(0, 0);
  EEPROM.commit();
}

void vTask1( void *pvParameters )
{
  for( ;; )
  {
    Serial.printf("vTask1 %d\n", xPortGetCoreID());
  Serial.printf("c'est la premiere tache 1", xPortGetCoreID());
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );
  }
}

void vTask2( void *pvParameters )
{
  for( ;; )
  {
    if (digitalRead(inter) == HIGH){
    Serial.println("- GPIO36 actif - ");
    erase_credentials();
    Serial.println("effacé");
    digitalWrite(output26, HIGH);
    delay(250);
    digitalWrite(output26, LOW);
    vTaskDelay( pdMS_TO_TICKS( 250 ) );
  }
  vTaskDelay( pdMS_TO_TICKS( 250 ) );
 }
}


void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  pinMode(output26, OUTPUT);
  pinMode(output13, OUTPUT);
  pinMode(output12, OUTPUT);
  pinMode(inter,INPUT_PULLDOWN);
  xTaskCreate(vTask1, "vTask1", 5000, NULL, 2, NULL);
  xTaskCreate(vTask2, "vTask2", 10000, NULL, 1, NULL);
WiFi.softAP("ESP");
Serial.println(WiFi.localIP()+"adresse ipsur le point d'accés");
// 





if (has_credentials()) {
  char ssidrecup[32], passwordrecup[32];
    load_credentials(ssidrecup, passwordrecup);
  Serial.println("identifant retrouvé"+String(ssidrecup));
  WiFi.begin(ssidrecup, passwordrecup);
    SPI.begin();  
   mfrc522.PCD_Init();

    while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.println("Connexion au WiFi... via ce qui est enregistré");
  }
  if (WiFi.status() == WL_CONNECTED)
  { 
    Serial.println("Connecte au wifi grace a ce qui a été enregistré");
    Serial.println(WiFi.localIP()+"adresse ip grace a ce qui a été enregistré");
  }
}

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
     Serial.println("html demande");
    request->send(200, "text/html", HTML);
  });
  
server.on(
    "/config",
    HTTP_POST,
    [](AsyncWebServerRequest * request){
    },
    NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      const size_t maxStringLength = 50;
      char ssid[maxStringLength];
      char password[maxStringLength];
      char identifier[maxStringLength];
      char secondaryPassword[maxStringLength];
      String requestData = "";
      for (size_t i = 0; i < len; i++) {
        requestData += (char)data[i];
      }
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, requestData);
      strncpy(ssid, doc["ssid"] | "", maxStringLength);
      strncpy(password, doc["password"] | "", maxStringLength);
      strncpy(identifier, doc["identifier"] | "", maxStringLength);
      strncpy(secondaryPassword, doc["secondaryPassword"] | "", maxStringLength);
      Serial.print("SSID: "); Serial.println(ssid);
      Serial.print("Password: "); Serial.println(password);
      Serial.print("Identifier: "); Serial.println(identifier);
      Serial.print("Secondary Password: "); Serial.println(secondaryPassword);
      request->send(200, "text/plain", requestData);

save_credentials(ssid, password);
Serial.print("enregistré");
WiFi.begin(ssid, password);
        SPI.begin();  
   mfrc522.PCD_Init();
    while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(output13, HIGH);
    delay(1000);
    Serial.println("Connexion au WiFi...");
        digitalWrite(output13, LOW);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connecte au wifi");
  }
  });

//  events.onConnect([](AsyncEventSourceClient *client){
//     if(client->lastId()){
//       Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
//     }
//     // send event with message "hello!", id current millis
//     // and set reconnect delay to 1 second
//     client->send("hello!", NULL, millis(), 10000);
//   });
//   server.addHandler(&events);

  server.begin();

}

void loop() {

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      Serial.println("tentative de lecture de carte");
    delay(5000);
    return;
  }
  Serial.print("Carte détectée avec le numéro de série : ");
  digitalWrite(  output12, HIGH);
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {  content.concat(String("0"));
    }
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("Content-Type", "application/json");
    String jsonPayload = "{\"fkidEtudiant\":\"" + content + "\"}";
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("HTTP Request failed. Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    digitalWrite(  output12, LOW);
  }
  delay(2000);
}