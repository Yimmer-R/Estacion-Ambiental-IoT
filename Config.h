#ifndef CONFIG_H
#define CONFIG_H

// =============================================
//   CONFIGURACIÓN WiFi (Wokwi usa WOKWI-GUEST)
// =============================================
const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// =============================================
//   CONFIGURACIÓN THINGSPEAK
//   1. Crea cuenta en thingspeak.com
//   2. New Channel -> agrega 3 fields:
//      Field 1: Luz
//      Field 2: Temperatura
//      Field 3: Humedad
//   3. Copia la Write API Key aquí
// =============================================
const char* THINGSPEAK_API_KEY = "U4OBDDJTSK7AC2B8";

// =============================================
//   PINES DEL ESP32
//   NOTA: Con WiFi activo SOLO usar ADC1
//   ADC1: GPIO 32,33,34,35,36,39
// =============================================
const int PIN_LDR    = 34;   // Analógico ADC1 (LDR)
const int PIN_DHT    = 4;    // Digital   (DHT22)
const int PIN_LED    = 2;    // Digital salida (LED alerta)
const int PIN_BOTON  = 15;   // Digital entrada (Botón reset)

// =============================================
//   UMBRALES Y TIEMPOS
// =============================================
const float TEMP_ALERTA    = 30.0;   // °C — enciende LED si supera esto
const int   INTERVALO_MS   = 15000;  // 15 seg entre envíos a ThingSpeak
                                     // (límite gratuito: 1 envío cada 15 s)

#endif