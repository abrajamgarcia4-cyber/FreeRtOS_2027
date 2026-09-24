//#include <Arduino.h>

// Manejadores de las tareas
TaskHandle_t Tarea2_Handle = NULL;
TaskHandle_t Tarea3_Handle = NULL;

#define MAX_BUFFER 100

/* 1. Tarea 1: Recepción por puerto serial (Adaptada a ESP32/Arduino) */
void Tarea1(void *pvParameters) {
    char buffer_local[MAX_BUFFER];
    uint8_t index = 0;

    while (1) {
        // Lectura del puerto serie usando la API de Arduino
        if (Serial.available() > 0) {
            char c = Serial.read();
            
            // Al recibir un \n o \r
            if (c == '\n' || c == '\r') {
                if (index > 0) { 
                    buffer_local[index] = '\0';
                    size_t len = index + 1;

                    // Comprobar si hay suficiente memoria Heap disponible
                    if (xPortGetFreeHeapSize() >= len) {
                        // Reservar el espacio exacto de la cadena
                        char *ptr_mensaje = (char *)pvPortMalloc(len);
                        
                        if (ptr_mensaje != NULL) {
                            strcpy(ptr_mensaje, buffer_local);
                            // Enviar la dirección de memoria como un valor de 32 bits
                            xTaskNotify(Tarea2_Handle, (uint32_t)ptr_mensaje, eSetValueWithOverwrite);
                        }
                    } else {
                        Serial.println("Error: Memoria insuficiente para la cadena.");
                    }
                    index = 0; // Reiniciar buffer
                }
            } else {
                if (index < MAX_BUFFER - 1) {
                    buffer_local[index++] = c;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Necesario para no bloquear el Watchdog del ESP32
    }
}

/* 2. Tarea 2: Procesamiento */
void Tarea2(void *pvParameters) {
    uint32_t valor_notificacion;
    
    while (1) {
        // Espera la notificación usando xTaskNotifyWait().
        // No debe esperar eternamente (portMAX_DELAY), debe tener un timeout de 5 segundos.
        BaseType_t resultado = xTaskNotifyWait(
            0x00, 
            0xFFFFFFFF, 
            &valor_notificacion, 
            pdMS_TO_TICKS(5000)
        );

        if (resultado == pdTRUE) {
            // Al recibir una notificación, extrae la dirección de memoria del mensaje.
            char *ptr_mensaje = (char *)valor_notificacion;
            
            // Modifica la cadena en la misma memoria del Heap, deberá invertir el orden de la cadena.
            // Por ejemplo, recibe "HOLA" cambia el orden a "ALOH"[cite: 2].
            int len = strlen(ptr_mensaje);
            for (int i = 0; i < len / 2; i++) {
                char temp = ptr_mensaje[i];
                ptr_mensaje[i] = ptr_mensaje[len - 1 - i];
                ptr_mensaje[len - 1 - i] = temp;
            }
            
            // Una vez modificada, notifica a la Tarea 3 pasando de nuevo el puntero en el valor de la notificación[cite: 2].
            xTaskNotify(Tarea3_Handle, (uint32_t)ptr_mensaje, eSetValueWithOverwrite);
        } else {
            // Si pasan 5 segundos sin recibir mensajes, debe imprimir "Estado: Sistema inactivo, esperando comandos..." y volver a esperar[cite: 2].
            Serial.println("Estado: Sistema inactivo, esperando comandos...");
        }
    }
}

/* 3. Tarea 3: Transmisión por puerto serial */
void Tarea3(void *pvParameters) {
    uint32_t valor_notificacion;
    
    while (1) {
        // Espera indefinidamente la notificación de la Tarea 2[cite: 2].
        xTaskNotifyWait(0x00, 0xFFFFFFFF, &valor_notificacion, portMAX_DELAY);
        
        // Al recibirla, extrae el puntero y envía la cadena modificada al Monitor Serie[cite: 2].
        char *ptr_mensaje = (char *)valor_notificacion;
        Serial.print("Mensaje procesado: ");
        Serial.println(ptr_mensaje);
        
        // Imprime cuánta memoria Heap queda disponible en ese momento[cite: 2].
        Serial.print("Memoria Heap restante: ");
        Serial.println(xPortGetFreeHeapSize());
        
        // Finalmente, utiliza vPortFree() para liberar ese bloque exacto de memoria[cite: 2].
        vPortFree(ptr_mensaje);
    }
}

void setup() {
    // Inicializar puerto serial para el ESP32
    Serial.begin(115200);
    while (!Serial) { delay(10); } // Esperar conexión
    
    Serial.println("Sistema iniciado. Ingresa un mensaje:");

    // Crear las tareas con un tamaño de pila adecuado para el ESP32 (2048 bytes es seguro)
    xTaskCreate(Tarea1, "Recepcion", 2048, NULL, 1, NULL);
    xTaskCreate(Tarea2, "Procesamiento", 2048, NULL, 2, &Tarea2_Handle);
    xTaskCreate(Tarea3, "Salida", 2048, NULL, 3, &Tarea3_Handle);
}

void loop() {
    // En entornos con FreeRTOS bajo Arduino (como el ESP32), el loop() 
    // corre como una tarea de baja prioridad. Se puede eliminar.
    vTaskDelete(NULL); 
}