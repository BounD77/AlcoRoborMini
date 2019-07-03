/*
 * Датчики
 */


#define Temp_req_int 1000  //частота запросов температуры с датчиков



//выставленные пользователем значения для контроля
float Temp_top_set = 0.0;
float Temp_bot_set = 0.0;
float Temp_delta_set = 0.0;

//здесь храним данные с датчиков последние
float Temp_top = 0.0;
float Temp_bot = 0.0;


float Rounded_temp_top = 0.0;
float Rounded_temp_bot = 0.0;
float Temp_delta = 0.0;

 
//адреса датчиков температуры -
DeviceAddress Sensor_top;             // = { 0x28, 0xFF, 0xDC, 0xA6, 0xC3, 0x17, 0x4, 0xC3 }; 
DeviceAddress Sensor_bot;             // = { 0x28, 0xCC, 0xAF, 0xC5, 0x6, 0x0, 0x0, 0xC9 }; 



void StringToDeviceAddress(DeviceAddress da, String da_str){
  

  int str_len = da_str.length() + 1; 
  char char_array[str_len];
  da_str.toCharArray(char_array, str_len);
  char * pch = strtok (char_array," ,");

  int i = 0;
  while (pch != NULL)
  {
    char *end;
    da[i] = strtol(pch, &end, 16);//транслируем массив чар в 16-ти зн и записываем в DA
    pch = strtok (NULL, " ,"); //берем в строке след значение до разделителя
    i++;
  }

  DeviceAddressToString(da);

}




void Sensors_init() {

  String temp_top_addr_str = jsonRead(configSetup,"Term_top_addr");
  String temp_bot_addr_str = jsonRead(configSetup,"Term_bot_addr");


  //конвертим адрес термометров со строки настроек в тип DeviceAddress 
  StringToDeviceAddress(Sensor_top, temp_top_addr_str);
  StringToDeviceAddress(Sensor_bot, temp_bot_addr_str);



  sensors.begin();  

  TempSensorHandler();
  

  ts.add(0, Temp_req_int, [&](void*) { // Запустим задачу 0 с интервалом Temp_req_int(1000)
    TempSensorHandler();
  }, nullptr, true);

}


//обработка датчиков температуры- опрос и запись в json
void TempSensorHandler() {
    //опрашиваем датчики
    sensors.requestTemperatures();

    Temp_top = sensors.getTempC(Sensor_top);
    Temp_bot = sensors.getTempC(Sensor_bot);

   if(Temp_top < 0.0 || Temp_top > 120.0 || Temp_top == 85.0){
      Serial.print("Temp_top sensor has error value: ");
      Serial.print(Temp_top);
      Serial.print(" at ");
      Serial.println(GetTime());
      BlinkInfoLed(true);//мигаем красным инфо ледом
      SoketData ("Temp_top_error", (String)Temp_top, "-1");
      Temp_top = -1.0;
    }
    if(Temp_bot < 0.0 || Temp_bot > 120.0 || Temp_bot == 85.0){
      Serial.print("Temp_bot sensor has error value: ");
      Serial.print(Temp_bot);
      Serial.print(" at ");
      Serial.println(GetTime());
      SoketData ("Temp_bot_error", (String)Temp_bot, "-1");      
      BlinkInfoLed(true);//мигаем красным инфо ледом      
      Temp_bot = -1.0;
    }

  

    if(Temp_top > 0.0){
      Rounded_temp_top = TempRoundedOneDec(Temp_top);
      Temp_delta = Rounded_temp_top - Temp_top_set;
      String Rounded_temp_top_str = TempRoundedString(Rounded_temp_top);
      String Rounded_temp_delta_str = TempRoundedString(Temp_delta);
      SoketData ("Temp_top", Rounded_temp_top_str, jsonRead(configLiveJson,"Temp_top"));
      SoketData ("Temp_delta", Rounded_temp_delta_str, jsonRead(configLiveJson,"Temp_delta"));
      jsonWrite(configLiveJson, "Temp_top", Rounded_temp_top_str);
      jsonWrite(configLiveJson, "Temp_delta", Rounded_temp_delta_str);
   }

    if(Temp_bot > 0.0){
      Rounded_temp_bot = TempRoundedOneDec(Temp_bot);
      String Rounded_temp_bot_str = TempRoundedString(Rounded_temp_bot);
      SoketData ("Temp_bot", Rounded_temp_bot_str, jsonRead(configLiveJson,"Temp_bot"));
      jsonWrite(configLiveJson, "Temp_bot", Rounded_temp_bot_str);
   }

}


//========================== Вспомогательные функции ==================================
//округление градусов до одной десятой 
float TempRoundedOneDec(float t){
  int num_int = int(round(t * 10));
  return num_int / 10.0;
}

//отрезаем лишний ноль на конце при записи в с троку
String TempRoundedString(float t){
  String result = String(t);
  return result.substring(0, result.length() - 1);
}

//отрезаем в строке все за первой десятичной (без округления)
String CutStringOneDec(String s){

  int s_len = s.length();
  String res = "";
  for (int i = 0; i < s_len; i++) {
    if(s[i] != '.'){
      res  += s[i];
    }
    else{
      res += '.';
      if(i+1 < s_len){
        res += s[i+1];
      } else {
        res += "0";
      }
      break;
    }
  }

  return res;
}

// function to print a device address
String DeviceAddressToString(DeviceAddress deviceAddress)
{
  String res = "";
  
  for (uint8_t i = 0; i < 8; i++)
  {
    String unit = "0x";
    // zero pad the address if necessary
    if (deviceAddress[i] < 16){
      unit += "0";
    }
    unit += String(deviceAddress[i], HEX);
    if( i != 7){
      unit += ", ";
    }
    res += unit;
   }
   
  Serial.print("Print device address: ");   
  Serial.println(res);
  
  return res;
}

void RetrieveAllDeviceAddresses(){
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");

  byte countDA = sensors.getDeviceCount();
  DeviceAddress term_addr;
 
  Serial.print(countDA, DEC);
  Serial.println(" devices.");

  for(byte i = 0; i < countDA; i++){

    if(sensors.getAddress(term_addr, i)){
      jsonWrite(TermDAJson, (String)i, DeviceAddressToString(term_addr));        
    }else{
      Serial.printf("%s\n address not found!", i);
    }
    
  }
  
}









