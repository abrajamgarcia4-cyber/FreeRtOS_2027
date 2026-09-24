/*
  TAREA 1 (SemaforoTask): Semáforo con 3 LEDs (rojo, amarillo, verde)
                           con retardo ajustable mediante un potenciómetro.

  TAREA 2 (BlinkTask):    Parpadeo de un LED cada 20 ms.

  TAREA 3 (ControlTask):  Lee el puerto serie y controla las tareas 1 y 2:
                           'A' -> Suspende Tarea 1
                           'a' -> Reanuda Tarea 1
                           'B' -> Suspende Tarea 2
                           'b' -> Reanuda Tarea 2
                           '1' -> Elimina Tarea 1
                           '2' -> Elimina Tarea 2

  TAREA 4 (EstadoTask):   Al recibir 'E' muestra por el monitor serie
                           el estado de las tareas 1 y 2.

    LED Rojo     -> GPIO 25
    LED Amarillo -> GPIO 26
    LED Verde    -> GPIO 27
    LED Blink    -> GPIO 2  (LED integrado)
  ===========================================================
*/

// ---------- pines ----------
#define LED_ROJO      25
#define LED_AMARILLO  26
#define LED_VERDE     27
#define LED_BLINK     2

// ---------- Handles de las tareas ----------
TaskHandle_t Task1_Handle = NULL;   // Semáforo
TaskHandle_t Task2_Handle = NULL;   // Blink
TaskHandle_t Task3_Handle = NULL;   // Control serie
TaskHandle_t Task4_Handle = NULL;   // Estado

// ---------- Banderas para saber si las tareas existen (para no acceder a handles borrados) ----------
volatile bool task1Existe = true;
volatile bool task2Existe = true;

// ============================================================
// TAREA 1: Semáforo con retardo ajustable (potenciómetro)
// ============================================================
void SemaforoTask(void *pvParameters) {
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  for (;;) {
    int retardo = 500;  // Delay ajustable entre 200 y 3000 ms

    // Verde
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, LOW);
    vTaskDelay(pdMS_TO_TICKS(retardo));

    // Amarillo
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, HIGH);
    digitalWrite(LED_ROJO, LOW);
    vTaskDelay(pdMS_TO_TICKS(retardo / 3));

    // Rojo
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, HIGH);
    vTaskDelay(pdMS_TO_TICKS(retardo));
  }
}

// ============================================================
// TAREA 2: Blink cada 20 ms
// ============================================================
void BlinkTask(void *pvParameters) {
  pinMode(LED_BLINK, OUTPUT);

  for (;;) {
    digitalWrite(LED_BLINK, !digitalRead(LED_BLINK));
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// ============================================================
// TAREA 3: Control por puerto serie
// ============================================================
void ControlTask(void *pvParameters) {
  for (;;) {
    if (Serial.available() > 0) {
      char c = Serial.read();

      switch (c) {
        case 'A':  // Suspende Tarea 1
          if (task1Existe && Task1_Handle != NULL) {
            vTaskSuspend(Task1_Handle);
            Serial.println("Tarea 1 (Semaforo) SUSPENDIDA");
          } else {
            Serial.println("Tarea 1 no existe (ya fue eliminada)");
          }
          break;

        case 'a':  // Reanuda Tarea 1
          if (task1Existe && Task1_Handle != NULL) {
            vTaskResume(Task1_Handle);
            Serial.println("Tarea 1 (Semaforo) REANUDADA");
          } else {
            Serial.println("Tarea 1 no existe (ya fue eliminada)");
          }
          break;

        case 'B':  // Suspende Tarea 2
          if (task2Existe && Task2_Handle != NULL) {
            vTaskSuspend(Task2_Handle);
            Serial.println("Tarea 2 (Blink) SUSPENDIDA");
          } else {
            Serial.println("Tarea 2 no existe (ya fue eliminada)");
          }
          break;

        case 'b':  // Reanuda Tarea 2
          if (task2Existe && Task2_Handle != NULL) {
            vTaskResume(Task2_Handle);
            Serial.println("Tarea 2 (Blink) REANUDADA");
          } else {
            Serial.println("Tarea 2 no existe (ya fue eliminada)");
          }
          break;

        case '1':  // Elimina Tarea 1
          if (task1Existe && Task1_Handle != NULL) {
            vTaskDelete(Task1_Handle);
            Task1_Handle = NULL;
            task1Existe = false;
            // Apagamos los LEDs del semáforo al eliminar la tarea
            digitalWrite(LED_ROJO, LOW);
            digitalWrite(LED_AMARILLO, LOW);
            digitalWrite(LED_VERDE, LOW);
            Serial.println("Tarea 1 (Semaforo) ELIMINADA");
          } else {
            Serial.println("Tarea 1 ya habia sido eliminada");
          }
          break;

        case '2':  // Elimina Tarea 2
          if (task2Existe && Task2_Handle != NULL) {
            vTaskDelete(Task2_Handle);
            Task2_Handle = NULL;
            task2Existe = false;
            digitalWrite(LED_BLINK, LOW);
            Serial.println("Tarea 2 (Blink) ELIMINADA");
          } else {
            Serial.println("Tarea 2 ya habia sido eliminada");
          }
          break;

        default:
          // Cualquier otro caracter se ignora (incluye 'E', que la maneja la Tarea 4)
          break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Pequeña pausa para no saturar la CPU
  }
}

// ============================================================
// TAREA 4: Muestra el estado de las tareas al recibir 'E'
// ============================================================
void EstadoTask(void *pvParameters) {
  for (;;) {
    if (Serial.available() > 0) {
      char c = Serial.peek(); // Miramos sin quitar el dato del buffer

      if (c == 'E') {
        Serial.read(); // Ahora sí lo consumimos

        Serial.println("----- ESTADO DE LAS TAREAS -----");

        // Estado Tarea 1
        Serial.print("Tarea 1 (Semaforo): ");
        if (!task1Existe || Task1_Handle == NULL) {
          Serial.println("ELIMINADA");
        } else {
          eTaskState estado = eTaskGetState(Task1_Handle);
          Serial.println(estadoTexto(estado));
        }

        // Estado Tarea 2
        Serial.print("Tarea 2 (Blink): ");
        if (!task2Existe || Task2_Handle == NULL) {
          Serial.println("ELIMINADA");
        } else {
          eTaskState estado = eTaskGetState(Task2_Handle);
          Serial.println(estadoTexto(estado));
        }

        Serial.println("---------------------------------");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ============================================================
// Función auxiliar: convierte eTaskState a texto legible
// ============================================================
String estadoTexto(eTaskState estado) {
  switch (estado) {
    case eRunning:   return "EJECUTANDOSE";
    case eReady:     return "LISTA";
    case eBlocked:   return "BLOQUEADA";
    case eSuspended: return "SUSPENDIDA";
    case eDeleted:   return "ELIMINADA";
    default:         return "DESCONOCIDO";
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=========================================");
  Serial.println(" Sistema FreeRTOS - ESP32 - 4 Tareas");
  Serial.println("=========================================");
  Serial.println(" A/a : Suspender/Reanudar Tarea 1 (Semaforo)");
  Serial.println(" B/b : Suspender/Reanudar Tarea 2 (Blink)");
  Serial.println(" 1   : Eliminar Tarea 1");
  Serial.println(" 2   : Eliminar Tarea 2");
  Serial.println(" E   : Mostrar estado de las tareas");
  Serial.println("=========================================");

  // Creación de las 4 tareas
  xTaskCreate(SemaforoTask, "Tarea1_Semaforo", 2048, NULL, 1, &Task1_Handle);
  xTaskCreate(BlinkTask,    "Tarea2_Blink",    1024, NULL, 1, &Task2_Handle);
  xTaskCreate(ControlTask,  "Tarea3_Control",  2048, NULL, 2, &Task3_Handle);
  xTaskCreate(EstadoTask,   "Tarea4_Estado",   2048, NULL, 2, &Task4_Handle);
}

void loop() {
 
}
