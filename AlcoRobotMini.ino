#include "set.h"


void setup() {

 
    Serial.begin(115200);


    //8-ми сегментный LKM-1638
    LKM1638_init();

    //Запускаем файловую систему
    Serial.println("Start 4-FS");
    FS_init();
    Serial.println("Step7-FileConfig");
    configSetup = readFile("config.json", 4096);

    Serial.println(configSetup);
    Serial.println("Start 1-WIFI");
    //Запускаем WIFI
    WIFIinit();

    Serial.println("Start 8-Time");
    // Получаем время из сети
    Time_init();
    //Настраиваем и запускаем SSDP интерфейс
    Serial.println("Start 3-SSDP");
    SSDP_init();
    //Настраиваем и запускаем HTTP интерфейс
    Serial.println("Start 2-WebServer");
    HTTP_init();
    //Настраиваем и запускаем webSoket интерфейс
    Serial.println("Start 14-webSocket_init");
    webSocket_init();
    // Включаем Вывод времени и даты каждую секунду
    Serial.println("Start 13-sec_init");

    //передача времени
    Time_loop_init();
    //инициализация датчиков
    Sensors_init();
    //вывод инфы о чипе
    SetupESPChipInfo();

    tm.reset();
    DrawTemperature();    

    BrightnessDisp1638Init();

    SelectValveInit();

    SavedSetTempInit(); 

    Serial.println(configLiveJson);

    //ReadTempSensorAddr();
    
}

void loop() {

    ts.update(); //планировщик задач
    HTTP.handleClient();
    yield();
    webSocket.loop();
    dnsServer.processNextRequest();
    Alcorobot();

}


//собираем инфу о чипе
void SetupESPChipInfo() {
    sendOptions("flashChip", String(ESP.getFlashChipId(), HEX));
    sendOptions("ideFlashSize", ESP.getFlashChipSize());
    sendOptions("realFlashSize", ESP.getFlashChipRealSize());
    sendOptions("flashChipSpeed", ESP.getFlashChipSpeed() / 1000000);
    sendOptions("cpuFreqMHz", ESP.getCpuFreqMHz());
    FlashMode_t ideMode = ESP.getFlashChipMode();
    sendOptions("FreeSketchSpace", ESP.getFreeSketchSpace());
    sendOptions("flashChipMode", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
    sendOptions("mac", WiFi.macAddress().c_str());
    sendOptions("ip", WiFi.localIP().toString());
    sendOptions("mac", WiFi.macAddress().c_str());

}

