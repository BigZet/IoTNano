//ИМПОРТ БИБЛ
#include <ArduinoECCX08.h>
#include <utility/ECCX08JWS.h>
#include <ArduinoMqttClient.h>
#include <Arduino_JSON.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <Wire.h>

#include "arduino_secrets.h"
#include "start_config.h"

String clientId;

const char ssid[]        = SECRET_SSID; //Название сети
const char pass[]        = SECRET_PASS; //Пароль
const char projectId[]   = SECRET_PROJECT_ID; //ID проекта, есть в "информации о проекте"
const char cloudRegion[] = SECRET_CLOUD_REGION; //Регион IoT core девайса, можно увидеть в вкладке IoT Core/Devices
const char registryId[]  = SECRET_REGISTRY_ID;//Registry ID можно увидеть в вкладке IoT Core/Devices
const String deviceId    = SECRET_DEVICE_ID; //можно увидеть в вкладке IoT Core/Devices
const char broker[]      = BROKER; //адрес mqtt моста (брокера)

const uint8_t max_brightness = MAX_BRIGHTNESS;
byte ledBrightness = LED_BRIGHTNESS; //Options: 0=Off to 255=50mA
byte sampleAverage = SAMPLE_AVERAGE; //Options: 1, 2, 4, 8, 16, 32
byte ledMode = LED_MODE; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
byte sampleRate = SAMPLE_RATE; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = PULSE_WIDTH; //Options: 69, 118, 215, 411
int adcRange = ADC_RANGE; //Options: 2048, 4096, 8192, 16384

uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data


WiFiSSLClient wifiSslClient; //Объект зашифрованной WiFi сессии
MqttClient    mqttClient(wifiSslClient); //Объект MQTT клиента
MAX30105 particleSensor;// Объект сенсора

unsigned long lastMillis = 0; //Время с последнего действия

void setup() {
  Serial.begin(9600);//Запуск и открытие серийного порта для подключения по проводу и вывода в консоль
  while (!Serial); //Ждем пока не откроется порт

  String clientId = calculateClientId();
  initConnectModule();
  //initSensor();

}


void loop() {
  reConnect();
  sendAlive();
 
}
