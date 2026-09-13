#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const char msg[] = "hola mundo UPIIZ";

static TaskHandle_t tarea_1 = NULL;
static TaskHandle_t tarea_2 = NULL;

void tarea01(void *parameter){
  int msg_len = strlen(msg);

  while(1){
    Serial.println();
    for(int i =0; i< msg_len; i++){
      Serial.println(msg[i]);
      vTaskDelay(1/ portTICK_PERIOD_MS);
    }
    Serial.println();
    vTaskDelay(1000/ portTICK_PERIOD_MS);
  }
}

void tarea02(void *parameter){
  while(1){
    Serial.println('+');
    vTaskDelay(100/ portTICK_PERIOD_MS);

  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(1000/ portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--- Mensaje de inicio ---");

  xTaskCreatePinnedToCore(
    tarea01, //funcion a llamar
    "tarea01", //nombre de tarea
    1024, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    &tarea_1,
    //NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );

  xTaskCreatePinnedToCore(
    tarea02, //funcion a llamar
    "tarea02", //nombre de tarea
    1024, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    2, //prioridad
    &tarea_2, // identificador de tarea para controlar la tarea (manejador)
    //NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 3; i++){
    vTaskSuspend(tarea_2);
    vTaskDelay(2000/ portTICK_PERIOD_MS);
    vTaskResume(tarea_2);
    vTaskDelay(2000/ portTICK_PERIOD_MS);
  }
  if(tarea_1 != NULL){
    vTaskDelete(tarea_1);
    tarea_1 = NULL;
  }

}
