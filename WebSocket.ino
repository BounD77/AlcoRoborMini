void webSocket_init() {
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
    {
        Serial.println("web Socket disconnected");
    }
    break;
    case WStype_CONNECTED: // Событие происходит при подключении клиента
    {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        //Serial.println("web Socket Connected");
        //webSocket.sendTXT(num, configLiveJson); // Отправим в всю строку с данными используя номер клиента он в num
    }
    break;
    case WStype_TEXT: // Событие происходит при получении данных текстового формата из webSocket
    {
        Serial.print("Cmd length: ");
        Serial.println(length);

       if(length > 0 && length < 31) {
          CmdHandler(num, payload, length);
       }



        // webSocket.sendTXT(num, "message here"); Отправить любое сообщение как строку по номеру соединения
        // webSocket.broadcastTXT("message here"); Отрпвыить всем подключенным
    }
    break;
    case WStype_BIN:
    {   // Событие происходит при получении бинарных данных из webSocket
        // webSocket.sendBIN(num, payload, length);
    }
    break;
    }
}

void CmdHandler(uint8_t num, uint8_t * payload, size_t length){

  /////////////////////////// парсим команду и параметры /////////////////
  char chr[length];
  byte break_pos = 0;
  byte separ_pos = 0;

  for (int i = 0; i < length; i++) {
    if(payload[i] == 38){ //если символ &  - разделитель параметров в команде
      separ_pos = i;
    }
   
    if(payload[i] == 35){//если символ # - конец строки в команде
      
      Serial.print("char 35 found at ");
      Serial.println(i);
      break_pos = i;
      break;
    }
    chr[i] = (char)payload[i];

  }

  String cmd = String (chr);//привели к строке
  
  cmd = cmd.substring(0, break_pos); //отделили по признаку конца строки зерна от плевел
  //cmd.trim();
  String cmd_name = cmd.substring(0, separ_pos); //отделили имя команды
  String cmd_val = cmd.substring(separ_pos + 1, break_pos); //отделили значение команды

  if( cmd_name != ""){
    Serial.print("cmd_name: ");
    Serial.println(cmd_name);
  }
  if( cmd_val != ""){
    Serial.print("cmd_val: ");
    Serial.println(cmd_val);
  }
///////////// подготовка к отправке и отправка на исполнение //////////////////////////

  if(cmd_name == "br1638"){ //яркость экрана
   byte new_val = cmd_val.toInt();
   if(new_val >= 0 || new_val <=7){ 
    SetBrightnessDisp1638(new_val);  
    SoketData ("Disp1638br", String(new_val), jsonRead(configSetup,"Disp1638br"));
    //пишем в пзу
    jsonWrite(configSetup, "Disp1638br", String(new_val));
    saveConfig();                 // Функция сохранения данных во Flash
    jsonWrite(configLiveJson, "Disp1638br",  String(new_val));
   }

   
  } else if(cmd_name == "set_mode"){ // режим клапана отбора
    //SetValveMode(cmd_val.toInt(), false);  
  } else if(cmd_name == "new_temp_bot" || cmd_name == "new_temp_top" ||  cmd_name == "new_temp_delta" ){
    //SetTempHandler(cmd_name, cmd_val);
  } else if(cmd_name == "set_valve_t"){
    SetValveTimingHandler(cmd_val);
  } else 
  {
    //Serial.println("Cmd is bad");
    //webSocket.sendTXT(num, "bad_command");
  }

  
}


// Отправка данных в Socket всем получателям
// Параметры Имя ключа, Данные, Предыдущее значение
bool SoketData (String key, String data, String data_old)  {

    bool answ = false;
    if (data_old != data) {
        String broadcast = "{}";
        jsonWrite(broadcast, key, data);
        webSocket.broadcastTXT(broadcast);
        answ = true;
    }
    return answ;
}

