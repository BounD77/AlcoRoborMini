
//выставленные пользователем значения для контроля
/*float Temp_top_set = 0.0;
float Temp_bot_set = 0.0;
float Temp_delta_set = 0.0;

*/

//обработчик присыла новых уставок температур с сокетов
void SetTempHandler(String cmd_name, String cmd_val){

  cmd_val.trim();

  //парсим во флоат
  float new_temp_float = cmd_val.toFloat();
  Serial.print("new_temp_float: ");
  Serial.println(new_temp_float);

  //отрезаем от нового строкового значения все после 1-го знака после запятой
  String new_temp_str = CutStringOneDec(String(new_temp_float));
  
  if(cmd_name == "new_temp_delta" ){


      if(new_temp_float > 0.0 && new_temp_float < 10.0){
      
        String old_temp_delta_str = jsonRead(configSetup, "Temp_delta_set"); //старое получаем в строке шоб прально сравнить новым
      
        if(old_temp_delta_str != new_temp_str){// если старое значение не равно новому

          Temp_delta_set = new_temp_str.toFloat(); // новое сохраняем во float
          jsonWrite(configSetup, "Temp_delta_set", new_temp_str); 
//          jsonWrite(configLiveJson, "Temp_delta_set", new_temp_str);          
          SoketData ("Temp_delta_set", new_temp_str, old_temp_delta_str); // и новое и старое шлем
          saveConfig();
        }
        else
        {
            Serial.println("new_temp_delta is the same");
        }
      }else{// больше указанного диапазона
            Serial.print("Val is bad: ");
            Serial.println(new_temp_float);
      }
      
        
  } else if(new_temp_float > 10.0 && new_temp_float < 100.0){
  
    if(cmd_name == "new_temp_bot" ){

     String old_temp_bot_str = jsonRead(configSetup,"Temp_bot_set"); //старое получаем в строке шоб прально сравнить новым
      
      if(old_temp_bot_str != new_temp_str){// если старое значение не равно новому

        Temp_bot_set = new_temp_str.toFloat(); // новое сохраняем во float
        jsonWrite(configSetup, "Temp_bot_set", new_temp_str);
//        jsonWrite(configLiveJson, "Temp_bot_set", new_temp_str);            
        SoketData ("Temp_bot_set", new_temp_str, old_temp_bot_str); // и новое и старое шлем
        saveConfig();
      }
      else
      {
          Serial.println("new_temp_bot is the same");
      }
      

    }
      
    else if(cmd_name == "new_temp_top" ){

     String old_temp_top_str = jsonRead(configSetup,"Temp_top_set"); //старое получаем в строке шоб прально сравнить новым
      
      if(old_temp_top_str != new_temp_str){// если старое значение не равно новому

        Temp_top_set = new_temp_str.toFloat(); // новое сохраняем во float
        jsonWrite(configSetup, "Temp_top_set", new_temp_str);
//        jsonWrite(configLiveJson, "Temp_top_set",new_temp_str);          
        SoketData ("Temp_top_set", new_temp_str, old_temp_top_str); // и новое и старое шлем
        saveConfig();
      }
      else
      {
          Serial.println("new_temp_top is the same");
      }

        
    }

    else { //если команда не распознана
      Serial.print("Cmd is bad: ");
      Serial.println(cmd_name);
    }

  }
  else {// если значения переданной температуры выходят за пределы разумного :)
      Serial.print("Val is bad: ");
      Serial.println(new_temp_float);

  }
}
  


//Читаем сохраненные в ПЗУ значения температуры - уставки
void SavedSetTempInit(){

//читаем с ПЗУ сначала в строку шоб 2 раза не теребонькать ПЗУ
  String Temp_top_set_str = jsonRead(configSetup,"Temp_top_set");
  String Temp_bot_set_str = jsonRead(configSetup,"Temp_bot_set");
  String Temp_delta_set_str = jsonRead(configSetup,"Temp_delta_set");

//и парсим
  Temp_top_set =  Temp_top_set_str.toFloat();
  Temp_bot_set = Temp_bot_set_str.toFloat();
  Temp_delta_set = Temp_delta_set_str.toFloat();

  jsonWrite(configLiveJson, "Temp_top_set",Temp_top_set_str);    
  jsonWrite(configLiveJson, "Temp_bot_set", Temp_bot_set_str);    
  jsonWrite(configLiveJson, "Temp_delta_set", Temp_delta_set_str);
  
}


