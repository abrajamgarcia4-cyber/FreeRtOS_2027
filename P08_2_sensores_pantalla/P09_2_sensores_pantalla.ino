#include "Adafruit_VL53L0X.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
//#include <Arduino.h>

#if CONFIG_FRERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Pines I2C para el ESP32-S3 SuperMini
#define I2C_SDA 8
#define I2C_SCL 9

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

volatile float varible_compartida;
static SemaphoreHandle_t mutex;
volatile bool id_tarea= 0;

void Sensor_distancia(void *pvParameters) {
  VL53L0X_RangingMeasurementData_t medida;
  volatile float local_var;
  while (1) {
    if(xSemaphoreTake(mutex, 100 / portTICK_PERIOD_MS) == pdTRUE){
      // Comprueba si hay una lectura lista
      lox.rangingTest(&medida, false);
      if (medida.RangeStatus!= 4 && medida.RangeMilliMeter < 8000) {
        local_var = medida.RangeMilliMeter;
        varible_compartida = local_var;
        id_tarea = 1;
      }
      else{
        local_var = -1;
        varible_compartida = local_var;
        id_tarea = 1;
      }
      xSemaphoreGive(mutex);
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    else{
    }
    Serial.print(pcTaskGetName(NULL));
    Serial.print(" : ");
    Serial.println(varible_compartida);
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void Sensor_IMU(void *pvParameters) {
  volatile float local_var;
  while (1) {
    sensors_event_t a, g, temp;
    if(xSemaphoreTake(mutex, 100 / portTICK_PERIOD_MS) == pdTRUE){
      mpu.getEvent(&a, &g, &temp);
      local_var = a.acceleration.x;
      varible_compartida = local_var;
      id_tarea = 0;
      xSemaphoreGive(mutex);
    }
    else{
    }
    
    Serial.print(pcTaskGetName(NULL));
    Serial.print(" : ");
    Serial.println(varible_compartida);
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

void pantalla(void *pvParameters) {
  volatile float copia_varible_compartida;
  volatile float copia_varible_compartida_2;

  while (1) {
    if(xSemaphoreTake(mutex, 100 / portTICK_PERIOD_MS) == pdTRUE){
      if(id_tarea == 1){
        copia_varible_compartida = varible_compartida;
      }
      else if(id_tarea == 0){
        copia_varible_compartida_2 = varible_compartida;
      }
      xSemaphoreGive(mutex);
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("distancia - mm");
    display.print(copia_varible_compartida, 1);
    display.setCursor(0, 30);
    display.println("Accelerometer - m/s^2");
    display.print(copia_varible_compartida_2, 1);
    display.display();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  // Espera a que se abra el puerto serie (necesario en USB nativo del ESP32-S3)
  while (!Serial) {
    delay(1);
  }
  // 1. Inicializa primero tu bus I2C personalizado
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  mutex= xSemaphoreCreateMutex();
  // 2. CORRECCIÓN: Le pasamos la dirección por defecto (0x29) y falsos/verdaderos para debug,
  // pero lo importante es pasarle la referencia &Wire que ya tiene configurados tus pines 8 y 9.
  if (!lox.begin(0x29, false, &Wire)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1); 
  }
  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
  //vTaskDelay(500 / portTICK_PERIOD_MS);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

  // Inicia la medición continua
  //lox.startRangeContinuous();
  delay(100);
  //vTaskDelay(1000 / portTICK_PERIOD_MS); 
  Serial.println("----------codigo inicio-----------");
  

  xTaskCreatePinnedToCore(Sensor_distancia, "distancia", 4096, NULL, 1, NULL,app_cpu);
  xTaskCreatePinnedToCore(Sensor_IMU, "IMU", 4096, NULL, 1, NULL,app_cpu);
  xTaskCreatePinnedToCore(pantalla, "pantalla oled", 4096, NULL, 2, NULL,app_cpu);
  //vTaskStartScheduler();
}

void loop() {
}