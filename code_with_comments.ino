/*
  GCP (Google Cloud Platform) IoT Core WiFi

  This sketch securely connects to GCP IoT Core using MQTT over WiFi.
  It uses a private key stored in the ATECC508A and a JSON Web Token (JWT) with
  a JSON Web Signature (JWS).

  It publishes a message every 5 seconds to "/devices/{deviceId}/state" topic
  and subscribes to messages on the "/devices/{deviceId}/config" and
  "/devices/{deviceId}/commands/#" topics.

  The circuit:
  - Arduino MKR WiFi 1010 or MKR1000

  This example code is in the public domain.
*/
//ИМПОРТ БИБЛ
#include <ArduinoECCX08.h>
#include <utility/ECCX08JWS.h>
#include <ArduinoMqttClient.h>
#include <Arduino_JSON.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000

#include "arduino_secrets.h"


//ОПРЕДЕЛЕНИЕ ОБЪЕКТОВ И КОНСТАНТ
//Данные для входа
const char ssid[]        = SECRET_SSID; //Название сети
const char pass[]        = SECRET_PASS; //Пароль

const char projectId[]   = SECRET_PROJECT_ID; //ID проекта, есть в "информации о проекте"
const char cloudRegion[] = SECRET_CLOUD_REGION; //Регион IoT core девайса, можно увидеть в вкладке IoT Core/Devices
const char registryId[]  = SECRET_REGISTRY_ID;//Registry ID можно увидеть в вкладке IoT Core/Devices
const String deviceId    = SECRET_DEVICE_ID; //можно увидеть в вкладке IoT Core/Devices

const char broker[]      = "mqtt.googleapis.com"; //адрес mqtt моста (брокера)

WiFiSSLClient wifiSslClient; //Объект зашифрованной WiFi сессии
MqttClient    mqttClient(wifiSslClient); //Объект MQTT клиента
//исходник https://github.com/arduino-libraries/ArduinoMqttClient/blob/master/src/MqttClient.cpp

unsigned long lastMillis = 0; //Время с последнего действия


//НАЧАЛЬНЫЕ УСТАНОВКИ выполняются перед запуском mainloop'a "void loop()"
void setup() {
  Serial.begin(9600);//Запуск и открытие серийного порта для подключения по проводу и вывода в консоль
  //Считать и отправить данные по этому порту можно с любого терминала, например Putty, выбрав serial в настройках 
  while (!Serial); //Ждем пока не откроется порт

//Если не запустился/отсуствует WiFi module
  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);//Входим в бесконечный цикл, альтернатива завершению кода
  }

  // Подсчет client id для MQTT
  String clientId = calculateClientId();

  //Установка client id MQTT клиенту
  mqttClient.setId(clientId);


  //Установка хендлера (обработчика, функции которая выполняется при получении сообщения) в MQTT Client
  mqttClient.onMessage(onMessageReceived);
}

//MAINLOOP ЦИКЛ в котором и происходит основная работа устройства
void loop() {
  
  //Если еще не подключены к сети WiFi
  if (WiFi.status() != WL_CONNECTED) {
    //Подключаемся
    connectWiFi();
  }

  //Если еще MQTT клиент не соединился с брокером
  if (!mqttClient.connected()) {
    // Соединяемся
    connectMQTT();
  }

  //Вообще не совсем понятная штука. Посмотрел исходники там она просто чекает 
  //соединение и проверяет если ли какие новые сообщения
  mqttClient.poll();

  // Каждые 10 сек отправляет сообщение
  if (millis() - lastMillis > 10000) {
    //Запоминаем новую временную точку отсчета
    //millis() – Возвращает количество миллисекунд, прошедших с запуска
    lastMillis = millis();

    //Отправка сообщения
    publishMessage();
  }
}


//ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ

unsigned long getTime() {
  // С  NTP сервера через WiFi модуль узнаем время с  первого января 1970 года 
  return WiFi.getTime();
}

//Функция для подключения к WiFi сети
void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");


  //Пока не подключимся к сети по SSID и паролю
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    //При неудаче
    Serial.print(".");
    //Ждем 5 сек
    delay(5000);
  }
  Serial.println();

  //При удаче
  Serial.println("You're connected to the network");
  Serial.println();
}


//Подключение к брокеру MQTT
//Точнее не совсем к брокеру, а к серверу, подключение к брокеру как часть общего подключения
void connectMQTT() {
  Serial.print("Attempting to connect to MQTT broker: ");
  Serial.print(broker); //Адрес брокера. "mqtt.googleapis.com" по умолчанию.
  Serial.println(" ");

  while (!mqttClient.connected()) {
    //Подсчитываем JsonWebToken более подробно о нем читай ниже в определении calculateJWT()
    String jwt = calculateJWT();

    //Здесь используется JWT токен, а не пароль. Дело в том, что MQTT соединение по умолчанию не защищено, а потому необходимо использовать ключ ES256 зашифрованный в виде JsonWebToken
    //Перед установкой нужно отправить изначальный ключ на облако. Изначальный ключ на устройстве создается и выводится только один раз, сменить удалить его невозможно.
    //Всего можно установить 4 ключа.
    //Уже установлен один изначальный ключ:
    /*
-----BEGIN PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEPJWfMter+NJffcgM+/VkR+Eorhs98CHf2i4v/yZt
zDSmwrdpZVMPLqPUFtgu1RygPc5dyCAbE2wuKLkrW6PYww==
-----END PUBLIC KEY-----
     */
     
    //Полученный JWT токен устанавливаем в качестве пароля mqtt пользователя, имя пользователя оставляем пустым
    mqttClient.setUsernamePassword("", jwt);

    //Пытаемся подключиться к брокеру по 8883 порту (стандартный для защищенного mqtt)
    if (!mqttClient.connect(broker, 8883)) {
      // Неудача
      Serial.print(".");
      //Спим 5 сек
      delay(5000);
    }
  }
  Serial.println();
  //Удача
  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // Подписываемся на топики устройств
  // Да по изначальному плану у нас устройство выступает исключительно в виде издателя.
  // Однако можно реализовать удаленную настройку устройства по средством отправки сообщений на топики /devices/deviceId/config и /devices/deviceId/commands/#
  // Также нежелательно это убирать, т.к. объект mqttClient класса MqttClient (ссылка на исходник в самом начале) может использовать эти топики для связи с Google облаком
  mqttClient.subscribe("/devices/" + deviceId + "/config", 1);
  mqttClient.subscribe("/devices/" + deviceId + "/commands/#");
}


/* Идентификатор клиента MQTT. Брокер его использует чтобы определять конкретное устройство
 * и узнавать состояние устройства. 
 * ID должен быть уникалным в базе брокера.
 * В MQTT 3.1.1 уже можно оставлять пустым это поле, но тогда брокер не сможет узнавать
 * состояние устройства */

//Подсчет идентификатора клиента
String calculateClientId() {
  String clientId;

  //  Формат по которому строится этот ClientID
  //  projects/{project-id}/locations/{cloud-region}/registries/{registry-id}/devices/{device-id}
  //  Как видим обеспечивается максимальная уникальность id-шника

  clientId += "projects/";
  clientId += projectId;
  clientId += "/locations/";
  clientId += cloudRegion;
  clientId += "/registries/";
  clientId += registryId;
  clientId += "/devices/";
  clientId += deviceId;

  return clientId;
}


//Подсчет JWT токена, по факту это зашифрованный JSON пакет
String calculateJWT() {
  //узнаем текущее время, необходимо гуглу для корректного расчета срока действия подключения
  unsigned long now = getTime();
  
  // calculate the JWT, based on:
  //   https://cloud.google.com/iot/docs/how-tos/credentials/jwts
  JSONVar jwtHeader; //JsonVar определяет объект Json документа (скорее всего открывает json запись)
  JSONVar jwtClaim;
  
  //Задаем запись {'alg':'ES256', 'typ':'JWT'}
  jwtHeader["alg"] = "ES256";
  jwtHeader["typ"] = "JWT";

//Задаем запись {'aud':'название проекта', 'iat':'текущее время (время начала)', 'время истечения срока подключения'}
  jwtClaim["aud"] = projectId;
  jwtClaim["iat"] = now;
  jwtClaim["exp"] = now + (24L * 60L * 60L); // срок подключения равен 24 часам

  //Важный момент! Обращаемся к криптомодулю через классовый метод класса ECCX08JWS создается пакет с заголовком и вызовом, преобразованных в JSON строку.
  //Возвращаеем полученную зашифрованную последовательность.
  return ECCX08JWS.sign(0, JSON.stringify(jwtHeader), JSON.stringify(jwtClaim));
}


//Публикация сообщения на брокер
void publishMessage() {
  Serial.println("Publishing message");

  // send message, the Print interface can be used to set the message contents
  // В классе обхекта mqttClient есть собственный метод print() который задает содержание сообщения.
  // Каждое MQTT сообщение должно быть обрамлено в beginMessage("топик в который издаем сообщение")
  // и endMessage() формирует MQTT запрос и публикует его.
  mqttClient.beginMessage("/devices/" + deviceId + "/state");
  mqttClient.print("hello ");
  mqttClient.print(millis());
  mqttClient.endMessage();
}


//Хендлер, обработчик сообщений поступивших с топиков, на которые мы до этого подписались
void onMessageReceived(int messageSize) {
  // Печатаем топик с которого получили сообщение, длину сообщения в байтах
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  //Перенаправляем поток вывода с MQTT клиента на серийный порт, т.е. на консоль
  //Точнее не совсем буквально перенаправляем, но суть та же.
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();

  Serial.println();
}
