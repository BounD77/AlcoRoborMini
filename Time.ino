void Time_init() {
    HTTP.on("/Time", handle_Time);     // Синхронизировать время устройства по запросу вида /Time
    HTTP.on("/timeZone", handle_time_zone);    // Установка времянной зоны по запросу вида http://192.168.0.101/timeZone?timeZone=3
    timeSynch(jsonReadtoInt(configSetup, "timezone"));
}
void timeSynch(int zone) {
    if (WiFi.status() == WL_CONNECTED) {
        // Настройка соединения с NTP сервером
        configTime(zone * 3600, 0, "pool.ntp.org", "ru.pool.ntp.org");
        int i = 0;
        Serial.println("\nWaiting for time");
        while (!time(nullptr) && i < 10) {
            Serial.print(".");
            i++;
            delay(1000);
        }
        Serial.println("");
        Serial.println("ITime Ready!");
 //       Serial.println(GetTime());
 //       Serial.println(GetDate());
    }
}
// Установка параметров времянной зоны по запросу вида http://192.168.0.101/timeZone?timeZone=3
void handle_time_zone() {
    jsonWrite(configSetup, "timezone", (int)HTTP.arg("timeZone").toInt()); // Получаем значение timezone из запроса конвертируем в int сохраняем
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
}

void handle_Time() {
    timeSynch(jsonReadtoInt(configSetup, "timezone"));
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}

//переделал получение времени нормально, а не по-пианэрски :)
//Форматированный вывод только времени
String GetTime() {
    struct tm *u;
    char ch[10] = { 0 };
    time_t now = time(nullptr);
    u = localtime(&now);
    strftime(ch, 10, "%H:%M:%S", u);
    String answ (ch);
    return answ;
}
// форматированный вывод только даты
String GetDate() {
    struct tm *u;
    char ch[12] = { 0 };
    time_t now = time(nullptr);
    u = localtime(&now);
    strftime(ch, 12, "%d.%m.%Y", u);
    String answ (ch);
    return answ;
}

String GetTestTime() {
    struct tm *u;
    char ch[10] = { 0 };
    time_t now = time(nullptr);
/*
Serial.print("time_now: ");
Serial.println(String(now));
*/
  return String(now);
 

}

//=====================  Вывод времени и даты в /config.live.json и в сокет ==========

void Time_loop_init() {
   ts.add(1, Temp_req_int, [&](void*) { // Запустим задачу 1 с интервалом Temp_req_int
        // поместим данные для web страницы в json строку configLiveJson
        // Будем вызывать эту функцию каждый раз при запросе /config.live.json
        // jsonWrite(строка, "ключ", значение_число); Так можно дабавить или обнавить json значение ключа в строке
        // jsonWrite(строка, "ключ", "значение_текст");
       String timeS = GetTestTime();
//        String date = GetDate();

       SoketData ("time", timeS, " ");

//       SoketData ("time", timeS, jsonRead(configLiveJson,"time"));
//        SoketData ("date", date, jsonRead(configLiveJson,"date"));

        jsonWrite(configLiveJson, "time", timeS); // отправить время в configLiveJson
 //       jsonWrite(configLiveJson, "date", date); // отправить дату в configLiveJson
    }, nullptr, true);

}


