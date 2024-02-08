#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <EEPROM.h>


#include <ESPAsyncWebServer.h>


#define SS_PIN 5
#define RST_PIN 27



// String ssid;
// String pass;
// String ip;
// String gateway;


char *apiEndpoint = "http://saegeii.axel-cal.fr/api/post";  // url api serveur

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Créer l'objet MFRC522

bool wifidispo = false;
AsyncWebServer server(80);

const char* PARAM_MESSAGE = "message";

const char HTML[] PROGMEM = "<!doctype html>"
"<html lang=\"en\">"
  "<head>"
    "<meta charset=\"UTF-8\" />"

    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
    "<title>Vite + React</title>"
    "<script type=\"module\" crossorigin src=\"/plugin.js\"></script>"
    "<link rel=\"stylesheet\" crossorigin href=\"/style.css\">"
  "</head>"
  "<body>"
    "<div><h1>titre</h1></div>"
    "<div id=\"root\"></div>"
  "</body>"
"</html>";
 
const char JS[] PROGMEM ="document.addEventListener(\"DOMContentLoaded\", function () { const editableInfos = {}; const handleInputChange = (e) => { const { name, value } = e.target; editableInfos[name] = value; }; async function postData(url = \"\", donnees = {}) { const response = await fetch(url, { method: \"POST\" , cache: \"no-cache\", credentials: \"same-origin\", headers: { \"Content-Type\": \"application/json\", }, redirect: \"follow\", referrerPolicy: \"no-referrer\", body: JSON.stringify(donnees), }); return response.json(); } const handleSubmit = (event) => { event.preventDefault(); const formData = { ssid: editableInfos.ssid, password: editableInfos.password, idesp: editableInfos.idesp, mdpesp: editableInfos.mdpesp, }; postData('http://192.168.4.1/config', formData) .then((response) => { console.log(response.data); }) .catch((error) => { console.error(\"Erreur lors de la soumission du formulaire :\", error); }); console.log(formData); }; const rootElement = document.getElementById(\"root\"); const container = document.createElement(\"div\"); container.className = \"container\"; rootElement.appendChild(container); const h1 = document.createElement(\"h1\"); h1.textContent = \"ESP32 IOT page de configuration\"; container.appendChild(h1); const form = document.createElement(\"form\"); form.addEventListener(\"submit\", handleSubmit); form.className = \"etudiant-changement\"; container.appendChild(form); const wifiConfig = document.createElement(\"div\"); wifiConfig.className = \"wifi-config\"; form.appendChild(wifiConfig); const wifiHeader = document.createElement(\"h2\"); wifiHeader.textContent = \"Reseaux WiFi\"; wifiConfig.appendChild(wifiHeader); const wifiLabels = [\"SSID\", \"mot de passe wifi\"]; wifiLabels.forEach((label) => { const inputLabel = document.createElement(\"label\"); inputLabel.htmlFor = label.toLowerCase().replace(/\\s/g, \"\"); inputLabel.textContent = label; wifiConfig.appendChild(inputLabel); const inputField = document.createElement(\"input\"); inputField.type = \"text\"; inputField.id = label.toLowerCase().replace(/\\s/g, \"\"); inputField.name = label.toLowerCase().replace(/\\s/g, \"\"); inputField.value = editableInfos[label.toLowerCase().replace(/\\s/g, \"\")] || \"\"; inputField.addEventListener(\"change\", handleInputChange); inputField.className = \"input-field\"; wifiConfig.appendChild(inputField); }); const espNowConfig = document.createElement(\"div\"); espNowConfig.className = \"espnow-config\"; form.appendChild(espNowConfig); const espNowHeader = document.createElement(\"h2\"); espNowHeader.textContent = \"Reseaux ESPNow\"; espNowConfig.appendChild(espNowHeader); const espNowLabels = [\"Identifiant ESP\", \"mot de passe ESP\"]; espNowLabels.forEach((label) => { const inputLabel = document.createElement(\"label\"); inputLabel.htmlFor = label.toLowerCase().replace(/\\s/g, \"\"); inputLabel.textContent = label; espNowConfig.appendChild(inputLabel); const inputField = document.createElement(\"input\"); inputField.type = \"text\"; inputField.id = label.toLowerCase().replace(/\\s/g, \"\"); inputField.name = label.toLowerCase().replace(/\\s/g, \"\"); inputField.value = editableInfos[label.toLowerCase().replace(/\\s/g, \"\")] || \"\"; inputField.addEventListener(\"change\", handleInputChange); inputField.className = \"input-field\"; espNowConfig.appendChild(inputField); }); const submitButton = document.createElement(\"input\"); submitButton.type = \"submit\"; submitButton.className = \"submit-button\"; form.appendChild(submitButton); });";

const char CSS[] PROGMEM ="h1{background-color:red}";

char *ssid = "axel";
char *password = "12345678";



void setup() {
  Serial.begin(115200);  // Initialise la communication série
 
  // Serial.println("Connecté au WiFi");
  // Serial.println("Approchez une carte RFID...");


if (wifidispo){

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


}
else{
WiFi.softAP("ESP");



Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
     Serial.println("html demande");
    request->send(200, "text/html", HTML);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("css demande");
    request->send(200, "text/css", CSS);
  });

  server.on("/plugin.js", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("js demande");
    request->send(200, "text/javascript", JS);
  });



      server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("reception de donne");
    String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        Serial.println(message);
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  // Start server
  server.begin();

}
}

void loop() {


  if (wifidispo){

    

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      Serial.println("tentative de lecture de carte");
    delay(250);
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




  }
  else{
  Serial.print("en train de fair serveur\n");
  delay(5000);
  }
    
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
