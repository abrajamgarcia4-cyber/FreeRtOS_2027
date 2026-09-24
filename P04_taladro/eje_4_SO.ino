#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#define led 19  //verde bajar taldro
#define led2 20 //amarillo subir taladro
#define led3 21 //rojo girar motor
#define boton1 47

#define t_1 4000  //duracion de cada estado
#define t_2 200   //duracion parpadeo rapido
#define t_3 20    //retado de seguradad entre tareas
#define t_4 600    //duracion parpadeo lento

volatile int aux = 0; //variable del semaforo
volatile bool aux2 = 0; //variable del enceder motor
volatile bool aux3 = 0; //variable del cambio de rutina

void bajar_rapido(void *parameter){
  while(1){
    if(aux == 0){
      digitalWrite(led,1); // bajamos taladro rapidamente
      vTaskDelay(t_2/portTICK_PERIOD_MS);
      digitalWrite(led,0);
      vTaskDelay(t_2/portTICK_PERIOD_MS);
      Serial.println("bajar rapido");
    }
  }
  vTaskDelay(t_3/portTICK_PERIOD_MS);
}

void bajar_lento(void *parameter){
  while(1){
    if(aux == 1){
      digitalWrite(led,1); //bajamos taladro lentamente y gira taladro
      vTaskDelay(t_4/portTICK_PERIOD_MS);
      digitalWrite(led,0);
      vTaskDelay(t_4/portTICK_PERIOD_MS);
      Serial.println("bajar lento");
    }
  }
  vTaskDelay(t_3/portTICK_PERIOD_MS);
}

void subir_lento(void *parameter){
  while(1){
    if(aux == 2){
      digitalWrite(led2,1); //sube taladro lenta
      vTaskDelay(t_4/portTICK_PERIOD_MS);
      digitalWrite(led2,0);
      vTaskDelay(t_4/portTICK_PERIOD_MS);
      Serial.println("subir lento");
    }
  }
  vTaskDelay(t_3/portTICK_PERIOD_MS);
}

void subir_rapido(void *parameter){
  while(1){
    if(aux == 3){
      digitalWrite(led2,1); //sube taladro rapida
      vTaskDelay(t_2/portTICK_PERIOD_MS);
      digitalWrite(led2,0);
      vTaskDelay(t_2/portTICK_PERIOD_MS);
      Serial.println("subir rapido");
    }
  }
  vTaskDelay(t_3/portTICK_PERIOD_MS);
}

void encender_motor(void *parameter){
  while(1){
    if(aux2 == 1){
      digitalWrite(led3,1); //encender motor
    }
    else
      digitalWrite(led3,0);
  }
  Serial.println("encender motor");
  vTaskDelay(t_3/portTICK_PERIOD_MS);
}
void toggle_boton01(void *parameter){
  while(1){
    aux3 = digitalRead(boton1);
    digitalWrite(48,aux3);
    vTaskDelay(60/portTICK_PERIOD_MS);
  }
}
void toggle_semaforo(void *parameter){
  while(1){
    aux = 0; // bajar lento
    aux2 = 0; // apagar motor
    vTaskDelay(t_1/portTICK_PERIOD_MS);
    aux = 1; // bajar rapido
    aux2 = 1; //encender motor
    vTaskDelay(t_1/portTICK_PERIOD_MS);

    if(aux3 == 1){
      aux = 3; // subir lento
      aux2 = 1; //encender motor
      Serial.println("boton presionado");
      vTaskDelay(t_1/portTICK_PERIOD_MS);
      aux = 0;// bajar lento
      aux2 = 1; //encender motor
      vTaskDelay(t_1/portTICK_PERIOD_MS);
    }
    else{
      aux = 3; // subir lento
      aux2 = 1; //encender motor
      vTaskDelay(t_1/portTICK_PERIOD_MS);
    }
    aux = 2; // subir rapido
    aux2 = 0; // apagar motor
    vTaskDelay(t_1/portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(48,OUTPUT);
  pinMode(boton1,INPUT_PULLUP);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
    bajar_rapido, //funcion a llamar
    "bajar_rapido", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    bajar_lento, //funcion a llamar
    "bajar_lento", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    subir_lento, //funcion a llamar
    "subir_lento", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    subir_rapido, //funcion a llamar
    "subir_rapido", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    encender_motor, //funcion a llamar
    "encender_motor", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    1, //prioridad
    NULL, // identificador o manejador(task handler)
    app_cpu //nucleo
  );
  xTaskCreatePinnedToCore(
    toggle_boton01, //funcion a llamar
    "toggle_boton01", //nombre de tarea
    4096, //tamano de pila (stack size)
    NULL, //parametro entre tareas
    2, //prioridad
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
