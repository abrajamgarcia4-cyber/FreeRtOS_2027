#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void toggle_semaforo(void *parameter){
  while(1){
    int a= 1;
    int b[100];
    for(int i=0; i<100; i++){
      b[i] = a+1;
    }
    Serial.println(b[0]);
    Serial.println("-------------stack---------------");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));
    Serial.println("-------------heap---------------");
    Serial.println(xPortGetFreeHeapSize());
    int *ptr= (int*)pvPortMalloc(1024 * sizeof(int));
    //condicion para realizar alguna accion antes de que exista un desbordamineto
    if (ptr == NULL){
      Serial.println("sin espacio en memoria");
      vPortFree(NULL); // comentar
    }
    else{
      for(int i=0; i<1024; i++){
        ptr[i]=3;
      }
    }
    Serial.print("espacio libre despues de malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());
    vTaskDelay(500/ portTICK_PERIOD_MS);
  }
}
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  xTaskCreatePinnedToCore(
    toggle_semaforo, //funcion a llamar
    "toggle_semaforo", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  vTaskDelete(NULL);  //optimizar memoria
}

void loop() {
  // put your main code here, to run repeatedly:

}
