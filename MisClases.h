#ifndef MIS_CLASES_H
#define MIS_CLASES_H

#include <DHT.h>

// ===================================================
//  CLASE: Led
//  Controla un LED digital (alerta de temperatura)
// ===================================================
class Led {
private:
  int pin;
  bool encendido;

public:
  // Constructor: recibe el pin donde está conectado el LED
  Led(int p) {
    pin = p;
    encendido = false;
  }

  // Configura el pin como salida
  void begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  // Enciende el LED
  void encender() {
    digitalWrite(pin, HIGH);
    encendido = true;
  }

  // Apaga el LED
  void apagar() {
    digitalWrite(pin, LOW);
    encendido = false;
  }

  // Devuelve true si el LED está encendido
  bool estaEncendido() {
    return encendido;
  }
};


// ===================================================
//  CLASE: Boton
//  Lee un botón con anti-rebote (debounce)
// ===================================================
class Boton {
private:
  int  pin;
  bool estadoAnterior;
  unsigned long ultimoCambio;
  const unsigned long DEBOUNCE_MS = 50;  // tiempo anti-rebote

public:
  // Constructor: recibe el pin del botón
  Boton(int p) {
    pin = p;
    estadoAnterior = HIGH;  // pull-up interno -> reposo en HIGH
    ultimoCambio   = 0;
  }

  // Configura el pin con resistencia pull-up interna
  void begin() {
    pinMode(pin, INPUT_PULLUP);
  }

  // Devuelve true UNA SOLA VEZ cuando se presiona el botón
  // (flanco descendente con debounce)
  bool fuePresionado() {
    bool estadoActual = digitalRead(pin);
    if (estadoActual != estadoAnterior) {
      ultimoCambio  = millis();
      estadoAnterior = estadoActual;
    }
    if ((millis() - ultimoCambio) > DEBOUNCE_MS && estadoActual == LOW) {
      estadoAnterior = HIGH;  // reset para detectar siguiente pulsación
      return true;
    }
    return false;
  }
};


// ===================================================
//  CLASE: LDR
//  Lee un sensor de luz (fotorresistencia)
// ===================================================
class LDR {
private:
  int pin;

public:
  // Constructor: recibe el pin analógico
  LDR(int p) {
    pin = p;
  }

  // Configura el pin como entrada
  void begin() {
    pinMode(pin, INPUT);
  }

  // Devuelve el valor crudo ADC (0-4095 en ESP32, 12 bits)
  int leer() {
    return analogRead(pin);
  }

  // Devuelve el porcentaje de luz (0% = oscuro, 100% = máxima luz)
  float leerPorcentaje() {
    return (leer() / 4095.0) * 100.0;
  }
};


// ===================================================
//  CLASE: SensorDHT
//  Lee temperatura y humedad del sensor DHT22
// ===================================================
class SensorDHT {
private:
  DHT dht;        // objeto de la librería DHT
  int pin;

public:
  // Constructor: recibe el pin y tipo (DHT22 por defecto)
  SensorDHT(int p, int tipo = DHT22) : dht(p, tipo) {
    pin = p;
  }

  // Inicializa el sensor
  void begin() {
    dht.begin();
  }

  // Devuelve la temperatura en °C (NaN si hay error)
  float leerTemperatura() {
    float t = dht.readTemperature();
    if (isnan(t)) {
      Serial.println("[DHT] Error leyendo temperatura");
      return -999.0;
    }
    return t;
  }

  // Devuelve la humedad en % (NaN si hay error)
  float leerHumedad() {
    float h = dht.readHumidity();
    if (isnan(h)) {
      Serial.println("[DHT] Error leyendo humedad");
      return -999.0;
    }
    return h;
  }
};

#endif