void HTTP_init(void) {
   //jsonWrite(configLiveJson, "flashChip", String(ESP.getFlashChipId(), HEX));

   // -------------------построение графика по запросу вида /charts.json?data=A0&data2=stateLed
   /*     HTTP.on("/charts.json", HTTP_GET, []() {
     String message = "{";                       // создадим json на лету
        uint8_t j = HTTP.args();                    // получим количество аргументов
        for (uint8_t i = 0; i < j; i++) { // Будем читать все аргументы по порядку
            String nameArg = HTTP.argName(i);         // Возьмем имя аргумента и зададим массив с ключём по имени аргумента
            String keyArg = HTTP.arg(i);
            String value = jsonRead(configLiveJson, HTTP.arg(i)); // Считаем из configLiveJson значение с ключём keyArg
            if (value != "")  { // если значение есть добавим имя массива
                message += "\"" + nameArg + "\":["; // теперь в строке {"Имя аргумента":[
                message += value; // добавим данные в массив теперь в строке {"Имя аргумента":[value
                value = "";       // очистим value
            }
            message += "]"; // завершим массив теперь в строке {"Имя аргумента":[value]
            if (i<j-1) message += ","; // если элемент не последний добавит , теперь в строке {"Имя аргумента":[value],
        }
        message += "}";
        // теперь json строка полная
        jsonWrite(message, "points", 10); // зададим количество точек по умолчанию
        jsonWrite(message, "refresh", 3000); // зададим время обнавления графика по умолчанию
        HTTP.send(200, "application/json", message);
       
    });
*/
  //добавление адреса термометра
   HTTP.on("/new_term_s", HTTP_GET, []() {
      jsonWrite(configSetup, HTTP.arg("term_name"), HTTP.arg("term_addr"));
      saveConfig();
      HTTP.send(200, "application/json", configSetup);
    });

   HTTP.on("/TermDAJson", HTTP_GET, []() {
     RetrieveAllDeviceAddresses();    
     HTTP.send(200, "application/json", TermDAJson);
   });

   // ------------------ уставки времени для ШИМа клапана отбора-------

   HTTP.on("/set_valve_t", HTTP_GET, []() {

      Serial.println("/set_valve_t request detected!");

      String nameArg = HTTP.argName(0); //nameArg = screen keyArg = off
      String keyArg = HTTP.arg(0);

      Serial.print("keyArg = ");
      Serial.println(keyArg);

      Serial.print("nameArg = ");
      Serial.println(nameArg);

      SetValveTimeHandler(nameArg, keyArg);
      HTTP.send(200, "application/json", AssembleConfigLiveJson());

   });

   //------------------------остальные команды ----------------------------

   HTTP.on("/cmd", HTTP_GET, []() {
      Serial.println("/cmd request detected!");

      String nameArg = HTTP.argName(0); //nameArg = screen keyArg = off
      String keyArg = HTTP.arg(0);

      Serial.print("keyArg = ");
      Serial.println(keyArg);

      Serial.print("nameArg = ");
      Serial.println(nameArg);
      if (nameArg == "new_temp_bot" || nameArg == "new_temp_top" || nameArg == "new_temp_delta") {
         SetTempHandler(nameArg, keyArg);
         HTTP.send(200, "application/json", AssembleConfigLiveJson());
      } else if (nameArg == "set_mode") {

         byte new_mode = keyArg.toInt();

         if (new_mode >= 0 || new_mode < 4) {
            SetValveMode(new_mode, false);
            HTTP.send(200, "application/json", AssembleConfigLiveJson());
            //HTTP.send(200, "text / plain", "ok");
         } else {
            HTTP.send(200, "text / plain", "BadRequest");
         }

      } else if (nameArg == "br1638") {
         byte new_val = keyArg.toInt();

         if (new_val >= 0 || new_val <= 7) {
            SetBrightnessDisp1638(new_val);
            //            SoketData ("Disp1638br", String(new_val), jsonRead(configSetup,"Disp1638br"));
            jsonWrite(configSetup, "Disp1638br", String(new_val));
            saveConfig();
            jsonWrite(configLiveJson, "Disp1638br", String(new_val));
            HTTP.send(200, "application/json", AssembleConfigLiveJson());
         } else {
            HTTP.send(200, "text / plain", "BadRequest");
         }
      } else {
         HTTP.send(200, "text / plain", "Cmd not found");
      }

   });

   // --------------------Выдаем данные configOptions
   HTTP.on("/config.options.json", HTTP_GET, []() {
      FSInfo fs_info;
      SPIFFS.info(fs_info);
      sendOptions("totalBytes", fs_info.totalBytes);
      sendOptions("usedBytes", fs_info.usedBytes);
      sendOptions("blockSize", fs_info.blockSize);
      sendOptions("pageSize", fs_info.pageSize);
      sendOptions("maxOpenFiles", fs_info.maxOpenFiles);
      sendOptions("maxPathLength", fs_info.maxPathLength);
      HTTP.send(200, "application/json", configOptions);

   });

   // --------------------Выдаем данные configLiveJson
   HTTP.on("/config.live.json", HTTP_GET, []() {
      HTTP.send(200, "application/json", AssembleConfigLiveJson());
   });
   // -------------------Выдаем данные configSetup
   HTTP.on("/config.setup.json", HTTP_GET, []() {
      HTTP.send(200, "application/json", configSetup);
   });
   // -------------------Перезагрузка модуля
   HTTP.on("/restart", HTTP_GET, []() {
      String restart = HTTP.arg("device"); // Получаем значение device из запроса
      if (restart == "ok") { // Если значение равно Ок
         HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
         ESP.restart(); // перезагружаем модуль
      } else { // иначе
         HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
      }
   });
   // Добавляем функцию Update для перезаписи прошивки по WiFi при 1М(256K SPIFFS) и выше
   //httpUpdater.setup(&HTTP);
   // Запускаем HTTP сервер
   HTTP.begin();
}

String AssembleConfigLiveJson() {

//   jsonWrite(configLiveJson, "Is_valve_closed", String(Is_valve_closed));
   jsonWrite(configLiveJson, "Mode", String(Mode));
   jsonWrite(configLiveJson, "Count_Down_valve_ON", String(On_time_valve));
   jsonWrite(configLiveJson, "Temp_top_set", TempRoundedString(Temp_top_set));
   jsonWrite(configLiveJson, "Temp_bot_set", TempRoundedString(Temp_bot_set));
   jsonWrite(configLiveJson, "Temp_delta_set", TempRoundedString(Temp_delta_set));
   jsonWrite(configLiveJson, "tail_came", String(Is_tail_came));
   jsonWrite(configLiveJson, "delta_info", String(Is_valve_closed_by_DELTA));

   jsonWrite(configLiveJson, "sel_v_on_sec", ValveTimingFormatter(Valve_set_on_sec));
   jsonWrite(configLiveJson, "sel_v_on_min", ValveTimingFormatter(Valve_set_on_min));
   jsonWrite(configLiveJson, "sel_v_on_hour", ValveTimingFormatter(Valve_set_on_hour));
   jsonWrite(configLiveJson, "sel_v_off_sec", ValveTimingFormatter(Valve_set_off_sec));
   jsonWrite(configLiveJson, "sel_v_off_min", ValveTimingFormatter(Valve_set_off_min));
   jsonWrite(configLiveJson, "sel_v_off_hour", ValveTimingFormatter(Valve_set_off_hour));

   jsonWrite(configLiveJson, "Cicles_on", String(On_time_valve_total_cicle));

   Serial.print("ConfigLiveJson: ");
   Serial.println(configLiveJson);

   return configLiveJson;
}
