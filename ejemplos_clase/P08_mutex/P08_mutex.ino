//#include <Arduino.h>
#if CONFIG_FRERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static int varible_compartida = 0;
static SemaphoreHandle_t mutex;


void incTarea(void *pvParameters) {
  int local_var;
  while (1) {
    if(xSemaphoreTake(mutex,0) == pdTRUE){
      local_var = varible_compartida;
      local_var++;
      vTaskDelay(random(100,500) / portTICK_PERIOD_MS);
      varible_compartida = local_var;
      Serial.print(pcTaskGetName(NULL));
      Serial.print(" : ");
      Serial.println(varible_compartida);
      xSemaphoreGive(mutex);
      taskYIELD();
    }
    else{

    }
  }
}


void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS); 
  Serial.println("----------codigo inicio-----------");
  mutex= xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(incTarea, "Tarea", 4096, NULL, 1, NULL,app_cpu);
  xTaskCreatePinnedToCore(incTarea, "Tarea 2", 4096, NULL, 1, NULL,app_cpu);
  vTaskDelete(NULL);
}

void loop() {
}