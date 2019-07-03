bool Is_valve_closed = 0;
byte Mode = 0;

byte Valve_set_on_hour = 0;
byte Valve_set_on_min = 0;
byte Valve_set_on_sec = 0;
byte Valve_set_off_hour = 0;
byte Valve_set_off_min = 0;
byte Valve_set_off_sec = 0;

void SelectValveInit() {

  pinMode(SelectValvePin, OUTPUT);
  CloseSelectValve();
  Valve_set_on_hour = (jsonRead(configSetup, "sel_v_on_hour")).toInt();
  Valve_set_on_min = (jsonRead(configSetup, "sel_v_on_min")).toInt();
  Valve_set_on_sec = (jsonRead(configSetup, "sel_v_on_sec")).toInt();
  Valve_set_off_hour = (jsonRead(configSetup, "sel_v_off_hour")).toInt();
  Valve_set_off_min = (jsonRead(configSetup, "sel_v_off_min")).toInt();
  Valve_set_off_sec = (jsonRead(configSetup, "sel_v_off_sec")).toInt();
/*
  TailLabelSwitcher(false);
  DeltaLabelSwitcher(false);
  Switch_PWM_on(true);
*/
  /*
    jsonWrite(configLiveJson, "sel_v_on_sec", ValveTimingFormatter(Valve_set_on_sec));
    jsonWrite(configLiveJson, "sel_v_on_min", ValveTimingFormatter(Valve_set_on_min));
    jsonWrite(configLiveJson, "sel_v_on_hour", ValveTimingFormatter(Valve_set_on_hour));
    jsonWrite(configLiveJson, "sel_v_off_sec", ValveTimingFormatter(Valve_set_off_sec));
    jsonWrite(configLiveJson, "sel_v_off_min", ValveTimingFormatter(Valve_set_off_min));
    jsonWrite(configLiveJson, "sel_v_off_hour", ValveTimingFormatter(Valve_set_off_hour));
  */

  //  jsonWrite(configLiveJson, "Is_valve_closed", String(Is_valve_closed));
  //  jsonWrite(configLiveJson, "tail_came", "0");
  // jsonWrite(configLiveJson, "delta_info", "1");  

  Mode = (jsonRead(configSetup, "Mode")).toInt();
  Serial.print("init mode: ");
  Serial.println(Mode);
/*  if (Mode == 7) {
    SetValveMode(0, true);
  }
  */
  SetValveMode(Mode, true);

}

//обработка команды на сокет на изменение тайминга клапанов
void SetValveTimingHandler(String cmd_val) {

  byte separ = cmd_val.indexOf('=');

  String param = cmd_val.substring(0, separ);
  String val = cmd_val.substring(separ + 1);
  Serial.print("param: ");
  Serial.println(param);
  Serial.print("val: ");
  Serial.println(val);

  byte val_b = val.toInt();
  if (val_b > 59) {
    val_b = 59;
  }
  if (param == "sel_v_on_sec") {

    if (Valve_set_on_sec != val_b) { // новое не равно старому

      SoketData("sel_v_on_sec", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_on_sec));
      Valve_set_on_sec = val_b;
      jsonWrite(configSetup, "sel_v_on_sec", ValveTimingFormatter(Valve_set_on_sec));
      saveConfig();
      //     jsonWrite(configLiveJson, "sel_v_on_sec", ValveTimingFormatter(Valve_set_on_sec));
    }

  } else if (param == "sel_v_on_min") {
    if (Valve_set_on_min != val_b) {

      Serial.println("Valve_set_on_min!");

      SoketData("sel_v_on_min", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_on_min));
      Valve_set_on_min = val_b;
      jsonWrite(configSetup, "sel_v_on_min", ValveTimingFormatter(val_b));
      saveConfig();
      //     jsonWrite(configLiveJson, "sel_v_on_min", ValveTimingFormatter(Valve_set_on_min));
    } else {
      Serial.print("ELSE! Valve_set_on_min != ");
      Serial.println(Valve_set_on_min);
      Serial.print("val_b = ");
      Serial.println(val_b);

    }

  } else if (param == "sel_v_on_hour") {
    if (Valve_set_on_hour != val_b) {

      SoketData("sel_v_on_hour", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_on_hour));
      Valve_set_on_hour = val_b;
      jsonWrite(configSetup, "sel_v_on_hour", ValveTimingFormatter(val_b));
      saveConfig();
      //      jsonWrite(configLiveJson, "sel_v_on_hour", ValveTimingFormatter(Valve_set_on_hour));
    }

  } else if (param == "sel_v_off_sec") {
    if (Valve_set_off_sec != val_b) {

      SoketData("sel_v_off_sec", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_off_sec));
      Valve_set_off_sec = val_b;
      jsonWrite(configSetup, "sel_v_off_sec", ValveTimingFormatter(val_b));
      saveConfig();
      //      jsonWrite(configLiveJson, "sel_v_off_sec", ValveTimingFormatter(Valve_set_off_sec));
    }

  } else if (param == "sel_v_off_min") {
    if (Valve_set_off_min != val_b) {

      SoketData("sel_v_off_min", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_off_min));
      Valve_set_off_min = val_b;
      jsonWrite(configSetup, "sel_v_off_min", ValveTimingFormatter(val_b));
      saveConfig();
      //     jsonWrite(configLiveJson, "sel_v_off_min", ValveTimingFormatter(Valve_set_off_min));
    }
  } else if (param == "sel_v_off_hour") {
    if (Valve_set_off_hour != val_b) {

      SoketData("sel_v_off_hour", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_off_hour));
      Valve_set_off_hour = val_b;
      jsonWrite(configSetup, "sel_v_off_hour", ValveTimingFormatter(val_b));
      saveConfig();
      //      jsonWrite(configLiveJson, "sel_v_off_hour", ValveTimingFormatter(Valve_set_off_hour));
    }
  }

  Recount_valve_time();

}

String ValveTimingFormatter(byte val) {

  String answ = String(val);
  if (val < 10) {
    answ = "0" + answ;
  }
  return answ;
}

void SetValveMode(byte new_mode, bool is_init_call) {

  String prev_mode_str = "";
  byte prev_mode = 7;

  if (!is_init_call) {
    prev_mode_str = jsonRead(configSetup, "Mode");
    prev_mode = prev_mode_str.toInt();
  }

  Serial.print("new_mode: ");
  Serial.println(new_mode);

  Serial.print("prev_mode: ");
  Serial.println(prev_mode);

  if (prev_mode != new_mode) {

    //   SoketData ("Mode", String(new_mode), String(prev_mode));  
    //    jsonWrite(configLiveJson, "Mode", String(Mode));                 
    /*
     * система поддерживает на данный момент 4 режима: 
     * 0- клапан всегда закрыт, работает как показометр
     * 1 - с ШИМ отобором (проверка на дельту верхнего градусника и проверка нижнего 
     * на разницу с установленной и отбор по ШИМу)
     * 2 - Без ШИМ- отбора - просто клапан открыт, но с проверкой на дельту верхнего 
     * градусника и проверка нижнего на разницу с установленной)
     * 3- клапан всегда открыт, проверок не произовадится никаких - показометр
     */

    SetLedMode(new_mode);

    switch (new_mode) {
    case 0: //всегда закрыт
      Mode = 0;
      CloseSelectValve();
      //TailLabelSwitcher(false);
      //DeltaLabelSwitcher(false);      
      SoketData("Mode", "0", prev_mode_str);
      //      jsonWrite(configLiveJson, "Mode", "0");
      jsonWrite(configSetup, "Mode", "0");
      saveConfig();
      break;

    case 1:
      Mode = 1;
      TailLabelSwitcher(false);
      DeltaLabelSwitcher(false);
      Switch_PWM_on(true);
      jsonWrite(configSetup, "Mode", "1");      
      saveConfig();
      //SoketData("tail_came", "0", "1");      
      SoketData("Mode", "1", prev_mode_str);
      break;

    case 2: //всегда открыт
      Mode = 2;
      TailLabelSwitcher(false);// сбрасываем приход хвостов при переключении на режим ВСЕГДА ОТКРЫТ
      //DeltaLabelSwitcher(false);      
      OpenSelectValve();
      SoketData("Mode", "2", prev_mode_str);
      //      jsonWrite(configLiveJson, "Mode", "3");
      jsonWrite(configSetup, "Mode", "2");
      saveConfig();
      break;

    }

  }

}

//открыть клапан!
void OpenSelectValve() {
  if (Is_valve_closed) { //если клапан закрыт - открываем
    Is_valve_closed = 0;
    digitalWrite(SelectValvePin, LOW); //открываем
    //SoketData("Is_valve_closed", "0", "1");
    jsonWrite(configLiveJson, "Is_valve_closed", "0");
    Serial.print("Clapan has opened at ");
    Serial.println(GetTime());    
    tm.setLED(0, 2);
    delay(400);
    SoketData("Is_valve_closed", "0", "1");
    
  }

}

//закрыть клапан!
void CloseSelectValve() {
  if (!Is_valve_closed) { //если клапан не закрыт - закрываем
    Is_valve_closed = 1;
    digitalWrite(SelectValvePin, HIGH); //закрываем
    //SoketData("Is_valve_closed", "1", "0");
    jsonWrite(configLiveJson, "Is_valve_closed", "1");
    Serial.print("Clapan has closed at ");
    Serial.println(GetTime());
    tm.setLED(0, 1);
    delay(400);
    SoketData("Is_valve_closed", "1", "0");
  }
}

//обработка команды на сокет на изменение тайминга клапанов
void SetValveTimeHandler(String param, String val) {

  byte val_b = val.toInt();
  if (val_b > 59) {
    val_b = 59;
  }
  
  if (param == "sel_v_on_sec") {

    if (Valve_set_on_sec != val_b) { // новое не равно старому

      if ((Valve_set_on_hour + Valve_set_on_min + val_b) == 0) {
        val_b = 1;
      }

      SoketData("sel_v_on_sec", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_on_sec));
      Valve_set_on_sec = val_b;
      jsonWrite(configSetup, "sel_v_on_sec", ValveTimingFormatter(Valve_set_on_sec));
      saveConfig();

    }

  } else if (param == "sel_v_on_min") {
    if (Valve_set_on_min != val_b) {

      if ((Valve_set_on_hour + val_b + Valve_set_on_sec) == 0) {
        val_b = 1;
      }

      SoketData("sel_v_on_min", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_on_min));
      Valve_set_on_min = val_b;
      jsonWrite(configSetup, "sel_v_on_min", ValveTimingFormatter(val_b));
      saveConfig();
    }
  } else if (param == "sel_v_on_hour") {
    if (Valve_set_on_hour != val_b) {

      if ((val_b + Valve_set_on_min + Valve_set_on_sec) == 0) {
        val_b = 1;
      }

      SoketData("sel_v_on_hour", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_on_hour));
      Valve_set_on_hour = val_b;
      jsonWrite(configSetup, "sel_v_on_hour", ValveTimingFormatter(val_b));
      saveConfig();
    }

  } else if (param == "sel_v_off_sec") {
    if (Valve_set_off_sec != val_b) {

      if ((Valve_set_off_hour + Valve_set_off_min + val_b) == 0) {
        val_b = 1;
      }

      SoketData("sel_v_off_sec", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_off_sec));
      Valve_set_off_sec = val_b;
      jsonWrite(configSetup, "sel_v_off_sec", ValveTimingFormatter(val_b));
      saveConfig();
    }

  } else if (param == "sel_v_off_min") {
    if (Valve_set_off_min != val_b) {

      if ((Valve_set_off_hour + val_b + Valve_set_off_sec) == 0) {
        val_b = 1;
      }

      SoketData("sel_v_off_min", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_off_min));
      Valve_set_off_min = val_b;
      jsonWrite(configSetup, "sel_v_off_min", ValveTimingFormatter(val_b));
      saveConfig();

    }
  } else if (param == "sel_v_off_hour") {
    if (Valve_set_off_hour != val_b) {

      if ((val_b + Valve_set_off_min + Valve_set_off_sec) == 0) {
        val_b = 1;
      }

      SoketData("sel_v_off_hour", ValveTimingFormatter(val_b), ValveTimingFormatter(Valve_set_off_hour));
      Valve_set_off_hour = val_b;
      jsonWrite(configSetup, "sel_v_off_hour", ValveTimingFormatter(val_b));
      saveConfig();

    }
  }

  Recount_valve_time();

}


