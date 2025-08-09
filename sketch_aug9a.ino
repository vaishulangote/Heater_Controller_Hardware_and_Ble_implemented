#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// --- PIN CONFIG ---
#define TEMP_PIN          4    // Temp sensor
#define HEATER_LED_PIN    5    // Heater LED (red)
#define STATUS_LED_PIN    6    // Status LED (green)
#define BUZZER_PIN        7    // Buzzer

// --- TEMP LIMITS ---
#define TEMP_LOW          22.0   // Heater ON below this
#define TEMP_HIGH         25.0   // Heater OFF above this
#define TEMP_OVERHEAT     40.0   // Safety limit

// --- HEATER STATES ---
enum HeaterState { IDLE, HEATING, STABILIZING, TARGET, OVERHEAT };
volatile HeaterState currentState = IDLE; // shared between tasks

// --- TASK HANDLES ---
TaskHandle_t sensorTaskHandle;
TaskHandle_t bleTaskHandle;
BLEAdvertising *pAdvertising;

// state name array for printing / BLE
const char* stateNames[] = { "Idle", "Heating", "Stabilizing", "Target", "Overheat" };

// =================================================================
// TASK 1: Read temp and update state machine
// =================================================================
void sensorTask(void *pvParameters) {
  Serial.println("Sensor task started");
  for (;;) {
    int raw = analogRead(TEMP_PIN);
    float voltage = (raw / 4095.0) * 2.5; // ADC -> voltage
    float tempC = (voltage - 0.5) * 100.0;

    if (currentState != OVERHEAT) {
      if (tempC >= TEMP_OVERHEAT) {
        currentState = OVERHEAT;
      } else {
        switch (currentState) {
          case IDLE:
            if (tempC < TEMP_LOW) currentState = HEATING;
            break;
          case HEATING:
            if (tempC >= TEMP_LOW) currentState = STABILIZING;
            break;
          case STABILIZING:
            if (tempC >= TEMP_HIGH) currentState = TARGET;
            else if (tempC < TEMP_LOW) currentState = HEATING;
            break;
          case TARGET:
            if (tempC < TEMP_LOW) currentState = HEATING;
            break;
          default: break;
        }
      }
    }

    // outputs
    bool heaterOn = (currentState == HEATING || currentState == STABILIZING);
    digitalWrite(HEATER_LED_PIN, heaterOn);
    digitalWrite(STATUS_LED_PIN, currentState == TARGET);

    if (currentState == OVERHEAT) tone(BUZZER_PIN, 1500, 500);
    else noTone(BUZZER_PIN);

    // log
    Serial.printf("Temp: %.1f C | State: %s | Heater: %s\n",
                  tempC, stateNames[currentState], heaterOn ? "ON" : "OFF");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// =================================================================
// TASK 2: BLE status update
// =================================================================
void bleTask(void *pvParameters) {
  Serial.println("BLE task started");
  for (;;) {
    char msg[30];
    snprintf(msg, sizeof(msg), "State: %s", stateNames[currentState]);

    BLEAdvertisementData adv;
    adv.setName("HeaterControl-C3");
    adv.setShortName(msg);
    pAdvertising->setAdvertisementData(adv);

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

// =================================================================
// SETUP
// =================================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("--- Heater Control Boot ---");

  pinMode(HEATER_LED_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT);

  BLEDevice::init("HeaterControl-C3");
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("BLE adv started");

  xTaskCreate(sensorTask, "Sensor", 4096, NULL, 1, &sensorTaskHandle);
  xTaskCreate(bleTask, "BLE", 2048, NULL, 1, &bleTaskHandle);
}

// =================================================================
// LOOP (not used much here)
// =================================================================
void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}