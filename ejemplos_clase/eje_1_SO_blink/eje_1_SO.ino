#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#define led 2
#define led2 22

#define t_1 550
#define t_2 330


void toggle_led01(void *parameter){
  while(1){
    digitalWrite(led,1);
    vTaskDelay(t_1/portTICK_PERIOD_MS);
    digitalWrite(led,0);
    vTaskDelay(t_1/portTICK_PERIOD_MS);
  }
}

void toggle_led02(void *parameter){
  while(1){
    digitalWrite(led2,1);
    vTaskDelay(t_2/portTICK_PERIOD_MS);
    digitalWrite(led2,0);
    vTaskDelay(t_2/portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
  xTaskCreatePinnedToCore(
    toggle_led01, //funcion a llamar
    "toggle_led01", //nombre de tarea
    1024, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    toggle_led02, //funcion a llamar
    "toggle_led02", //nombre de tarea
    1024, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );

}

void loop() {
  // put your main code here, to run repeatedly:

}
