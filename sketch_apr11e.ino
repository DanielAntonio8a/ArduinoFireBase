#include <WiFi.h>
#include <FirebaseESP32.h>

// Configuración de WiFi
const char* ssid = "FAMILIA OCHOA MEJIA 2do Piso";
const char* password = "Fmla8aPiso2";

// Configuración de Firebase
FirebaseConfig config;
FirebaseAuth auth;

// Pines de los interruptores DIP
int dipSwitchPins[] = {5, 18, 19, 21, 22, 23};
bool switchStates[] = {LOW, LOW, LOW, LOW, LOW, LOW};

FirebaseData firebaseData;

unsigned long lastDebounceTime[] = {0, 0, 0, 0, 0, 0};
unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  
  // Iniciar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" conectado!");

  // Configurar Firebase
  config.host = "https://bddlive-7e449-default-rtdb.firebaseio.com/";
  config.api_key = "AIzaSyDpJC93zdDaUl-zCaW9E7glbvGmEarOMLo";
  auth.user.email = "danielanto@uets.edu.ec";
  auth.user.password = "danielanto@uets.edu.ec";

  // Iniciar Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Configurar los pines de los interruptores DIP como entradas
  for (int i = 0; i < 6; i++) {
    pinMode(dipSwitchPins[i], INPUT_PULLUP);
  }
}

void loop() {
  unsigned long currentTime = millis();

  for (int i = 0; i < 6; i++) {
    int reading = digitalRead(dipSwitchPins[i]);
    if (reading != switchStates[i]) {
      lastDebounceTime[i] = currentTime;
    }
    if ((currentTime - lastDebounceTime[i]) > debounceDelay) {
      if (reading != switchStates[i]) {
        switchStates[i] = reading;
        // Realizar acciones cuando el estado del interruptor DIP cambia
        String path = "/Hogar/Interruptor" + String(i + 1); // Cambia esto para coincidir con tus necesidades
        updateFirebase(path, switchStates[i] == HIGH);
      }
    }
  }

  delay(10); // Pequeña pausa para estabilizar la lectura del interruptor
}

void updateFirebase(String path, bool state) {
  Firebase.setBool(firebaseData, path, state);
  if (firebaseData.dataType() == "boolean") {
    Serial.println("Estado actualizado en Firebase: " + path);
  }
}