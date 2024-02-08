
document.addEventListener("DOMContentLoaded", function () {
  const editableInfos = {};

  const handleInputChange = (e) => {
    const { name, value } = e.target;
    editableInfos[name] = value;
  };

  async function postData(url = "", donnees = {}) {

    const response = await fetch(url, {
      method: "POST",
      mode: "no-cors",
      cache: "no-cache",
      credentials: "same-origin",
      headers: {
        "Content-Type": "application/json",

      },
      redirect: "follow",
      referrerPolicy: "no-referrer",
      body: JSON.stringify(donnees), 
    });
    return response.json();
  }

  const handleSubmit = (event) => {
    event.preventDefault();

    const formData = {
      ssid: editableInfos.ssid,
      password: editableInfos.password,
      idesp: editableInfos.idesp,
      mdpesp: editableInfos.mdpesp,
    };

    postData('http://192.168.4.1/config', formData)
      .then((response) => {
        console.log(response.data);
      })
      .catch((error) => {
        console.error("Erreur lors de la soumission du formulaire :", error);
      });

    console.log(formData);
  };

  const rootElement = document.getElementById("root");
  const container = document.createElement("div");
  container.className = "container";
  rootElement.appendChild(container);

  const h1 = document.createElement("h1");
  h1.textContent = "ESP32 IOT page de configuration";
  container.appendChild(h1);

  const form = document.createElement("form");
  form.addEventListener("submit", handleSubmit);
  form.className = "etudiant-changement";
  container.appendChild(form);

  const wifiConfig = document.createElement("div");
  wifiConfig.className = "wifi-config";
  form.appendChild(wifiConfig);

  const wifiHeader = document.createElement("h2");
  wifiHeader.textContent = "Reseaux WiFi";
  wifiConfig.appendChild(wifiHeader);

  const wifiLabels = ["SSID", "mot de passe wifi"];
  wifiLabels.forEach((label) => {
    const inputLabel = document.createElement("label");
    inputLabel.htmlFor = label.toLowerCase().replace(/\s/g, "");
    inputLabel.textContent = label;
    wifiConfig.appendChild(inputLabel);

    const inputField = document.createElement("input");
    inputField.type = "text";
    inputField.id = label.toLowerCase().replace(/\s/g, "");
    inputField.name = label.toLowerCase().replace(/\s/g, "");
    inputField.value = editableInfos[label.toLowerCase().replace(/\s/g, "")] || "";
    inputField.addEventListener("change", handleInputChange);
    inputField.className = "input-field";
    wifiConfig.appendChild(inputField);
  });



  const espNowConfig = document.createElement("div");
  espNowConfig.className = "espnow-config";
  form.appendChild(espNowConfig);

  const espNowHeader = document.createElement("h2");
  espNowHeader.textContent = "Reseaux ESPNow";
  espNowConfig.appendChild(espNowHeader);

  const espNowLabels = ["Identifiant ESP", "mot de passe ESP"];
  espNowLabels.forEach((label) => {
    const inputLabel = document.createElement("label");
    inputLabel.htmlFor = label.toLowerCase().replace(/\s/g, "");
    inputLabel.textContent = label;
    espNowConfig.appendChild(inputLabel);

    const inputField = document.createElement("input");
    inputField.type = "text";
    inputField.id = label.toLowerCase().replace(/\s/g, "");
    inputField.name = label.toLowerCase().replace(/\s/g, "");
    inputField.value = editableInfos[label.toLowerCase().replace(/\s/g, "")] || "";
    inputField.addEventListener("change", handleInputChange);
    inputField.className = "input-field";
    espNowConfig.appendChild(inputField);
  });



  const submitButton = document.createElement("input");
  submitButton.type = "submit";
  submitButton.className = "submit-button";
  form.appendChild(submitButton);
});

