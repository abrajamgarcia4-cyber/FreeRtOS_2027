//#include <Arduino.h>

// Manejadores de las tareas
TaskHandle_t Tarea2_Handle = NULL;
TaskHandle_t Tarea3_Handle = NULL;
String global[100];

/* 1. Tarea 1: Recepción por puerto serial (Adaptada a ESP32/Arduino) */
void Tarea1(void *pvParameters) {
  while (1) {
    for(int i=0; i<50;i++){
      global[i]= String() + i + "a  ";
      Serial.print(global[i]);
    }
    Serial.println();
    //vTaskDelay(pdMS_TO_TICKS(10)); // Necesario para no bloquear el Watchdog del ESP32
  }
}

/* 2. Tarea 2: Procesamiento */
void Tarea2(void *pvParameters) {
  while (1) {
    for(int x=0; x<50;x++){
      global[x]= String() + x + "b  ";
      Serial.print(global[x]);
    }
    Serial.println();
    //vTaskDelay(pdMS_TO_TICKS(10)); // Necesario para no bloquear el Watchdog del ESP32
  }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); } // Esperar conexión
    
    // Crear las tareas 
    xTaskCreate(Tarea2, "Recepcion", 2048, NULL, 1, NULL);
    xTaskCreate(Tarea1, "Procesamiento", 2048, NULL, 1, NULL);
}

void loop() {
}