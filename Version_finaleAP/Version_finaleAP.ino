#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include <ESPAsyncWebServer.h>


#define SS_PIN 5
#define RST_PIN 27
#define EEPROM_SIZE 64
const int output26 = 26;
const int output13 = 13;

int inter = 36; //Initialisation de notre interrupteur à la patte 36


// String ssid;
// String pass;
// String ip;
// String gateway;


char *apiEndpoint = "http://saegeii.axel-cal.fr/api/post";  // url api serveur

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Créer l'objet MFRC522

bool wifidispo = false;
AsyncWebServer server(80);

const char* PARAM_MESSAGE = "message";

const char HTML[] PROGMEM = "<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <title>Formulaire Wi-Fi</title>\n"
"    <style>\n"
"        body {\n"
"            font-family: Arial, sans-serif;\n"
"            margin: 0;\n"
"            padding: 0;\n"
"            box-sizing: border-box;\n"
"        }\n"
"\n"
"        header {\n"
"            background-color: #333;\n"
"            color: #fff;\n"
"            text-align: center;\n"
"            padding: 10px;\n"
"        }\n"
"\n"
"        section {\n"
"            margin: 20px;\n"
"        }\n"
"\n"
"        form {\n"
"            display: grid;\n"
"            gap: 10px;\n"
"        }\n"
"\n"
"        label {\n"
"            display: block;\n"
"            margin-bottom: 5px;\n"
"        }\n"
"\n"
"        input {\n"
"            padding: 8px;\n"
"        }\n"
"\n"
"        button {\n"
"            padding: 10px;\n"
"            background-color: #333;\n"
"            color: #fff;\n"
"            border: none;\n"
"            cursor: pointer;\n"
"        }\n"
"    </style>\n"
"</head>\n"
"<body>\n"
"\n"
"    <header>\n"
"        <h1>Configuration Wi-Fi</h1>\n"
"    </header>\n"
"\n"
"    <section>\n"
"        <h2>Wi-Fi</h2>\n"
"        <form id=\"NetworkForm\">\n"
"            <label for=\"ssid\">SSID:</label>\n"
"            <input type=\"text\" id=\"ssid\" name=\"ssid\" required>\n"
"\n"
"            <label for=\"password\">Mot de passe:</label>\n"
"            <input type=\"password\" id=\"password\" name=\"password\" required>\n"
"\n"
"\n"
"        <h2>Réseau Secondaire</h2>\n"
"\n"
"            <label for=\"identifier\">Identifiant:</label>\n"
"            <input type=\"text\" id=\"identifier\" name=\"identifier\" required>\n"
"\n"
"            <label for=\"secondaryPassword\">Mot de passe:</label>\n"
"            <input type=\"password\" id=\"secondaryPassword\" name=\"secondaryPassword\" required>\n"
"\n"
"            <button type=\"button\" onclick=\"submitForm('NetworkForm')\">Valider</button>\n"
"        </form>\n"
"    </section>\n"
"\n"
"    <script>\n"
"        function submitForm(formId) {\n"
"            const form = document.getElementById(formId);\n"
"            const formData = new FormData(form);\n"
"            const jsonData = {};\n"
"\n"
"            formData.forEach((value, key) => {\n"
"                jsonData[key] = value;\n"
"            });\n"
"\n"
"            const url = 'http://192.168.4.1/config';\n"
"            fetch(url, {\n"
"                method: 'POST',\n"
"                headers: {\n"
"                    'Content-Type': 'application/json',\n"
"                },\n"
"                body: JSON.stringify(jsonData),\n"
"            })\n"
"            .then(response => response.text())\n"
"            .then(data => {\n"
"                console.log('Réponse de lAPI:', data);\n"
"\n"
"            })\n"
"            .catch(error => {\n"
"                console.error('Erreur lors de lenvoi des données:', error);\n"
"\n"
"            });\n"
"        }\n"
"    </script>\n"
"\n"
"</body>\n"
"</html>\n"
"\n"

;
 


// const char JS[] PROGMEM ="document.addEventListener(\"DOMContentLoaded\", function () {"
// "const editableInfos = {};";


// const char CSS[] PROGMEM ="h1{"
// "background-color:red"
// "}";


bool has_credentials() {
  return EEPROM.read(0) == 0x42 /* credentials marker */;
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




void vTask1( void *pvParameters ) // Déclaration de la tâche 1
{
  for( ;; ) // 
  {
    Serial.printf("vTask1 %d\n", xPortGetCoreID());
  Serial.printf("c'est la premiere tache 1", xPortGetCoreID());
    




    
    vTaskDelay( pdMS_TO_TICKS( 5000 ) );
  }
}



void vTask2( void *pvParameters )
{
  for( ;; )
  {

    
    if (digitalRead(inter) == HIGH){
    Serial.println("- GPIO36 actif - ");
    //vTaskDelete( xTask2Handle );
    erase_credentials();
    Serial.println("éffacé");
    digitalWrite(output26, HIGH);
    digitalWrite(output13, HIGH);
    delay(250);

    digitalWrite(output26, LOW);
digitalWrite(output13, LOW);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
  }
  vTaskDelay( pdMS_TO_TICKS( 1000 ) );
 }
}


void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);  // Initialise la communication série
 
  // Serial.println("Connecté au WiFi");
  // Serial.println("Approchez une carte RFID...");
  pinMode(output26, OUTPUT);
  pinMode(output13, OUTPUT);
  pinMode(inter,INPUT);


WiFi.softAP("ESP");
if (has_credentials()) {
char ssidrecup[32], passwordrecup[32];
    load_credentials(ssidrecup, passwordrecup);
Serial.println("identifant retrouvé"+String(ssidrecup));

WiFi.begin(ssidrecup, passwordrecup); // Essaye de se connecter au point d'accès avec le ssid et le mot de passe renseignés
        // Initialise la communication SPI
   mfrc522.PCD_Init();  // Initialise le module RC522
SPI.begin();  


    while (WiFi.status() != WL_CONNECTED) { //Affiche toute les secondes "Connexion au WiFi..." tant que l'ESP32 n'y est pas connecté
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  if (WiFi.status() == WL_CONNECTED)
  { //Affiche toute les secondes "Connexion au WiFi..." tant que l'ESP32 n'y est pas connecté
    
    Serial.println("Connecte au wifi");
  }

}

Serial.println(WiFi.localIP());









  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
     Serial.println("html demande");
    request->send(200, "text/html", HTML);
  });
  


  // server.on("/plugin.js", HTTP_GET, [](AsyncWebServerRequest *request){
  //   Serial.println("js demande");
  //   request->send(200, "text/javascript", JS);
  // });



  //     server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
  //   Serial.println("reception de donne");
  //   String message;
  //       if (request->hasParam(PARAM_MESSAGE, true)) {
  //           message = request->getParam(PARAM_MESSAGE, true)->value();
  //       } else {
  //           message = "No message sent";
  //       }
  //       Serial.println(message);
  //   request->send(200, "text/plain", message);
  // });
server.on(
    "/config",
    HTTP_POST,
    [](AsyncWebServerRequest * request){
      // Rien à faire ici pour le moment
    },
    NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      
      // Définir la taille maximale des tableaux de caractères
      const size_t maxStringLength = 50;

      // Déclarer des tableaux de caractères pour stocker les valeurs
      char ssid[maxStringLength];
      char password[maxStringLength];
      char identifier[maxStringLength];
      char secondaryPassword[maxStringLength];

      // Convertir les données de la requête en une chaîne de caractères
      String requestData = "";
      for (size_t i = 0; i < len; i++) {
        requestData += (char)data[i];
      }

      // Utiliser la bibliothèque ArduinoJson pour extraire les valeurs
      DynamicJsonDocument doc(1024);  // Taille du document JSON, ajustez si nécessaire
      deserializeJson(doc, requestData);

      // Extraire les valeurs des champs JSON
      strncpy(ssid, doc["ssid"] | "", maxStringLength);
      strncpy(password, doc["password"] | "", maxStringLength);
      strncpy(identifier, doc["identifier"] | "", maxStringLength);
      strncpy(secondaryPassword, doc["secondaryPassword"] | "", maxStringLength);

      // Afficher les valeurs dans la console
      Serial.print("SSID: "); Serial.println(ssid);
      Serial.print("Password: "); Serial.println(password);
      Serial.print("Identifier: "); Serial.println(identifier);
      Serial.print("Secondary Password: "); Serial.println(secondaryPassword);







      // Envoyer une réponse au client
      request->send(200, "text/plain", requestData);


save_credentials(ssid, password);
Serial.print("enregistré");
WiFi.begin(ssid, password); // Essaye de se connecter au point d'accès avec le ssid et le mot de passe renseignés
        // Initialise la communication SPI
   mfrc522.PCD_Init();  // Initialise le module RC522
SPI.begin();  


    while (WiFi.status() != WL_CONNECTED) { //Affiche toute les secondes "Connexion au WiFi..." tant que l'ESP32 n'y est pas connecté
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  if (WiFi.status() == WL_CONNECTED)
  { //Affiche toute les secondes "Connexion au WiFi..." tant que l'ESP32 n'y est pas connecté
    
    Serial.println("Connecte au wifi");
  }




  });






  // Start server
  server.begin();

  xTaskCreate(vTask1, "vTask1", 5000, NULL, 2, NULL);
  //xTaskCreate(vTask2, "vTask2", 10000, NULL, 2, &xTask2Handle);
  xTaskCreate(vTask2, "vTask2", 10000, NULL, 1, NULL);


}


void loop() {




    

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      Serial.println("tentative de lecture de carte");
    delay(10000);
    return;
  }
  // Affiche le numéro de série de la carte
  Serial.print("Carte détectée avec le numéro de série : ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) { // Boucle qui pour chaque octet lu sur la carte le convertit hexadecimale et l'ajoute a la suite de content

    if (mfrc522.uid.uidByte[i] < 0x10) { // Lors de la conversion en hexadecimale, les zeros a gauche disparaissent. On rajoute un 0 lorsque le nombre a convertir est inferieur a 0x10
      content.concat(String("0"));
    }

    content.concat(String(mfrc522.uid.uidByte[i], HEX)); // On convertit l'octet en hexadecimale
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Définir l'URL de l'API
    http.begin(apiEndpoint);

    // Définir les en-têtes de la requête
    http.addHeader("Content-Type", "application/json");
    // Créer le contenu JSON à envoyer
    String jsonPayload = "{\"fkidEtudiant\":\"" + content + "\"}";

    // Effectuer la requête HTTP POST
    int httpResponseCode = http.POST(jsonPayload);

    // Vérifier la réponse de l'API
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("HTTP Request failed. Error code: ");
      Serial.println(httpResponseCode);
    }

    // Libérer les ressources de la requête
    http.end();
  }
  // Attendez quelques instants pour éviter une lecture continue
  delay(2000);





    
// // WiFi.begin(ssid, password); // Essaye de se connecter au point d'accès avec le ssid et le mot de passe renseignés
// //   SPI.begin();         // Initialise la communication SPI
// //   mfrc522.PCD_Init();  // Initialise le module RC522
// //   while (WiFi.status() != WL_CONNECTED) { //Affiche toute les secondes "Connexion au WiFi..." tant que l'ESP32 n'y est pas connecté
// //     delay(1000);
// //     Serial.println("Connexion au WiFi...");
// //   }



//   }

  // Recherche des cartes RFID
  

}
