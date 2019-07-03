#include <ESP8266WiFi.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step1-wifi
#include <ESP8266WebServer.h>   //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step2-webserver
#include <ESP8266SSDP.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step3-ssdp
#include <FS.h>                 //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step4-fswebserver
#include <ArduinoJson.h>        //Установить из менеджера библиотек. https://arduinojson.org/
#include <ESP8266HTTPUpdateServer.h>  //Содержится в пакете.  Видео с уроком http://esp8266-arduinoide.ru/step8-timeupdate/
#include <DNSServer.h> //Содержится в пакете.  // Для работы символьных имен в режиме AP отвечает на любой запрос например: 1.ru
#include <TickerScheduler.h>         //https://github.com/Toshik/TickerScheduler Видео с уроком http://esp8266-arduinoide.ru/step8-timeupdate/
#include <WebSocketsServer.h>    //https://github.com/Links2004/arduinoWebSockets 

// Объект для обновления с web страницы
ESP8266HTTPUpdateServer httpUpdater;

// Web интерфейс для устройства
ESP8266WebServer HTTP(80);

// Для файловой системы
File fsUploadFile;

// Для работы символьных имен в режиме AP
DNSServer dnsServer;

//Планировщик задач (Число задач)
TickerScheduler ts(3);

//Работа с временем
#include <time.h>

// Объект для  webSocket
WebSocketsServer webSocket = WebSocketsServer(81);

//датчики температуры
#include "OneWire.h"
#include "DallasTemperature.h"
#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//экран 8-сегм LKM1638
#include <TM1638lite.h>
TM1638lite tm(4, 16, 5);

//реле и клапан отбора
#define SelectValvePin 12

String configSetup = "{}"; // конфиг с сохраннен
String configLiveJson = "{}";  // данные для config.live.json
String configOptions = "{}"; // данные для всех страниц config.options.json
String TermDAJson = "{}"; // данные все найденным на шине oneWire термометрам


