# Проект "Измеритель сердечного ритма и уровня кислорода в крови"
Состоит из:
- Платформа **Arduino Nano IoT 33**. Всю информацию по модулям смотри [тут](http://wiki.amperka.ru/products:arduino-nano-33-iot)
- Модуль **MAX30102**. Информация по нему (документация вроде) [тут](https://github.com/BigZet/IoTNano/blob/main/MAX30102_doc.pdf)



Дополнительная информация по подключению, отправке данных, обработке и т.д.
- Документация по модулю **WiFiNINA** [тут](https://www.arduino.cc/en/Reference/WiFiNINA), используется для подключения к сети WiFi
- Исходник (там и документация скорее всего есть) по классу **MqttClient** [тут](https://github.com/arduino-libraries/ArduinoMqttClient/blob/master/src/MqttClient.cpp). Он нужен для подключения устройства к облаку Google как к mqtt-брокеру и публикации информации с датчиков.
- Исходник на **ECCX08** [тут](https://github.com/arduino-libraries/ArduinoECCX08) класс для управления крипто-модулем. Мы используем его для подписи JsonWebToken.




*Красивые фоточки:*
![Alt-текст](https://cdn.shopify.com/s/files/1/0438/4735/2471/products/ABX00027_03.front_e538f30f-0eaa-4055-82b9-1d7aa468b3ed_954x716.jpg?v=1629821421 "Arduino Nano IoT")
![Alt-текст](https://imgaz.staticbg.com/images/oaupload/ser1/banggood/images/78/55/068ee8da-4aa1-442b-b210-89a59abdcb79.JPG "MAX30102")
