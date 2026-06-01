/*
 * =====================================================
 *  Estación Ambiental IoT — ESP32 + ThingSpeak
 *  Actividad Final — Programación Orientada a Objetos
 * =====================================================
 *  Sensores: DHT22 (temp/humedad) + LDR (luz)
 *  Alerta:   LED enciende si Temperatura > 30°C
 *  Reset:    Botón apaga la alerta del LED
 *  Nube:     Envía datos a ThingSpeak cada 15 s
 *
 *  Librerías necesarias (instalar en PlatformIO/Arduino):
 *    - DHT sensor library  (Adafruit)
 *    - Adafruit Unified Sensor
 *    - ThingSpeak           (MathWorks)
 * =====================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include "Config.h"
#include "MisClases.h"

// -------------------------------------------------------
// Instancias de las clases (usando pines de Config.h)
// -------------------------------------------------------
Led       led(PIN_LED);
Boton     boton(PIN_BOTON);
LDR       ldr(PIN_LDR);
SensorDHT dht(PIN_DHT, DHT22);

// -------------------------------------------------------
// Variables de control de tiempo
// -------------------------------------------------------
WiFiClient  cliente;
unsigned long ultimoEnvio = 0;

// -------------------------------------------------------
// Función: conectar al WiFi
// -------------------------------------------------------
void conectarWiFi() {
  Serial.print("\n[WiFi] Conectando a: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Conectado!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] FALLO al conectar. Reiniciando...");
    ESP.restart();
  }
}

// -------------------------------------------------------
// SETUP — se ejecuta una sola vez al arrancar
// -------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("===========================================");
  Serial.println("   Estación Ambiental IoT — ESP32         ");
  Serial.println("===========================================");

  // Inicializar periféricos
  led.begin();
  boton.begin();
  ldr.begin();
  dht.begin();
  Serial.println("[OK] Sensores y actuadores inicializados");

  // Parpadeo de arranque: LED parpadea 3 veces
  for (int i = 0; i < 3; i++) {
    led.encender();
    delay(150);
    led.apagar();
    delay(150);
  }

  // Conectar WiFi e inicializar ThingSpeak
  conectarWiFi();
  ThingSpeak.begin(cliente);
  Serial.println("[OK] ThingSpeak inicializado\n");
}

// -------------------------------------------------------
// LOOP — se repite indefinidamente
// -------------------------------------------------------
void loop() {

  // --- 1. Leer sensores ---
  int   luz         = ldr.leer();                  // 0 - 4095
  float luzPorc     = ldr.leerPorcentaje();        // 0.0 - 100.0 %
  float temperatura = dht.leerTemperatura();       // °C
  float humedad     = dht.leerHumedad();           // %

  // --- 2. Lógica de alerta: LED enciende si temp > umbral ---
  if (temperatura != -999.0 && temperatura > TEMP_ALERTA) {
    if (!led.estaEncendido()) {
      led.encender();
      Serial.println("[ALERTA] Temperatura alta — LED encendido");
    }
  }

  // --- 3. Botón reinicia la alerta (apaga el LED) ---
  if (boton.fuePresionado()) {
    led.apagar();
    Serial.println("[RESET] Botón presionado — LED apagado");
  }

  // --- 4. Mostrar lecturas en monitor serial ---
  Serial.println("-------------------------------------------");
  Serial.print("[LDR]   Luz cruda: ");  Serial.print(luz);
  Serial.print("  |  Porcentaje: ");    Serial.print(luzPorc, 1);
  Serial.println(" %");

  if (temperatura != -999.0) {
    Serial.print("[DHT]   Temperatura: "); Serial.print(temperatura, 1);
    Serial.print(" °C  |  Humedad: ");     Serial.print(humedad, 1);
    Serial.println(" %");
  }
  Serial.print("[LED]   Estado: ");
  Serial.println(led.estaEncendido() ? "ENCENDIDO" : "apagado");

  // --- 5. Enviar a ThingSpeak cada INTERVALO_MS ---
  unsigned long ahora = millis();
  if (ahora - ultimoEnvio >= INTERVALO_MS) {

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFi] Conexión perdida — reconectando...");
      conectarWiFi();
    }

    // Asignar campos del canal ThingSpeak
    ThingSpeak.setField(1, (int)luzPorc);       // Field 1: Luz (%)
    ThingSpeak.setField(2, temperatura);         // Field 2: Temperatura (°C)
    ThingSpeak.setField(3, humedad);             // Field 3: Humedad (%)

    // Enviar y verificar respuesta
    int httpCode = ThingSpeak.writeFields(
      0,                    // Channel ID — ThingSpeak lo infiere de la API Key
      THINGSPEAK_API_KEY
    );

    if (httpCode == 200) {
      Serial.println("[ThingSpeak] ✓ Datos enviados correctamente");
    } else {
      Serial.print("[ThingSpeak] ✗ Error HTTP: ");
      Serial.println(httpCode);
    }

    ultimoEnvio = ahora;
  }

  delay(1000);  // Pausa 1 segundo entre lecturas locales
}