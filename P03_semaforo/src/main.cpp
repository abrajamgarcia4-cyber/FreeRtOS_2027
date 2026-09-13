#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#define led 19  //verde
#define led2 20 //amarillo
#define led3 21 //rojo

#define t_1 2000  //duracion de cada estado
#define t_2 200   //duracion parpadeo
#define t_3 100    //retado de seguradad entre tareas

volatile int aux = 0; //variable del semaforo

void toggle_led01(void *parameter){
  while(1){
    if(aux == 0){
      digitalWrite(led,1);
      //Serial.println("Led 1 encendido");
    }
    else
      digitalWrite(led,0);
  }
  vTaskDelay(t_2/portTICK_PERIOD_MS);
}

void toggle_led02(void *parameter){
  while(1){
    if(aux == 1){
      digitalWrite(led2,1);
      vTaskDelay(t_2/portTICK_PERIOD_MS);
      digitalWrite(led2,0);
      vTaskDelay(t_2/portTICK_PERIOD_MS);
      //Serial.println("Led 2 encendido");
    }
    else
      digitalWrite(led2,0);
  }
  vTaskDelay(t_2/portTICK_PERIOD_MS);
}

void toggle_led03(void *parameter){
  while(1){
    if(aux == 2){
      digitalWrite(led3,1);
      //Serial.println("Led 3 encendido");
    }
    else
    digitalWrite(led3,0);
  }
  vTaskDelay(t_2/portTICK_PERIOD_MS);
}

void toggle_semaforo(void *parameter){
  while(1){
    aux = 0;
    vTaskDelay(t_1/portTICK_PERIOD_MS);
    aux = 1;
    vTaskDelay(t_1/portTICK_PERIOD_MS);
    aux = 2;
    vTaskDelay(t_1/portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  //Serial.begin(115200);
  xTaskCreatePinnedToCore(
    toggle_led01, //funcion a llamar
    "toggle_led01", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    toggle_led02, //funcion a llamar
    "toggle_led02", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    toggle_led03, //funcion a llamar
    "toggle_led03", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    toggle_semaforo, //funcion a llamar
    "toggle_semaforo", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );

}

void loop() {
  // put your main code here, to run repeatedly:

}
