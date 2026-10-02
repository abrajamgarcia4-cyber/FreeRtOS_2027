//#include <Arduino.h>
#if CONFIG_FRERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

volatile int contador=0;

void Tarea1(void *pvParameters) {
  while (1) {
    for(int i=0; i<1000;i++){
      contador++;
      vTaskDelay(random(100,500) / portTICK_PERIOD_MS); 
    }
    Serial.print("t1:");
    Serial.println(contador);
    vTaskDelete(NULL);
  }
}

/* 2. Tarea 2: Procesamiento */
void Tarea2(void *pvParameters) {
  while (1) {
    for(int i=0; i<1000;i++){
      contador++;
      vTaskDelay(random(100,500) / portTICK_PERIOD_MS); 
    }
    Serial.print("t2:");
    Serial.println(contador);
    vTaskDelete(NULL);
    //vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } 

  xTaskCreatePinnedToCore(Tarea1, "Procesamiento", 4096, NULL, 1, NULL,app_cpu);
  xTaskCreatePinnedToCore(Tarea2, "Recepcion", 4096, NULL, 1, NULL,app_cpu);

}

void loop() {
}