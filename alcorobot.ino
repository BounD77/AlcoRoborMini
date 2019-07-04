/*
 * система поддерживает на данный момент 3 режима: 
 * 0- клапан всегда закрыт, работает как показометр
 * 1 - с ШИМ отобором (проверка на дельту верхнего градусника и проверка нижнего 
 * на разницу с установленной и отбор по ШИМу)
 * 2 - Без ШИМ- отбора - просто клапан открыт, но с проверкой на дельту верхнего 
 * градусника и проверка нижнего на разницу с установленной)
 * 3- клапан всегда открыт, проверок не произовадится никаких - показометр
 */

unsigned long prev_millis = 0;

unsigned long prev_tail_millis = 0;

bool Is_time_valve_ON = true;
bool Is_time_valve_OFF = false;

unsigned long On_time_valve = 0;
unsigned long Off_time_valve = 0;

unsigned long On_time_valve_total_cicle = 0;

long Count_Down_valve_ON = 0;
long Count_Down_valve_OFF = 0;

bool Is_close_by_NO_pwm = true; //если клапан закрыывает НЕ шим, т.е по хвостам закрыт или по  дельте
bool Is_tail_came = false; //если пришли хвосты - здесь храним

bool Is_valve_closed_by_DELTA = false;
//для переключения с других вкладок
void DeltaLabelSwitcher(bool new_val) {
  if (new_val) {

    Serial.println("Is_valve_closed_by_DELTA TRUE");
    Is_valve_closed_by_DELTA = true;
    SetLedIsDelta(true);
    // delay(300);
    SoketData("delta_info", "1", "0");
  } else {

    Serial.println("Is_valve_closed_by_DELTA FALSE");
    Is_valve_closed_by_DELTA = false;
    SetLedIsDelta(false);
    // delay(300);
    SoketData("delta_info", "0", "1");
  }

}

//для переключения с других вкладок
void TailLabelSwitcher(bool new_val) {
  if (new_val) {
    Is_tail_came = true;
    SetLedIsTailCame(true);
    SoketData("tail_came", "1", "0");
  } else {
    Is_tail_came = false;
    SetLedIsTailCame(false);
    SoketData("tail_came", "0", "1");
  }

}

void Recount_valve_time() {

  On_time_valve = Valve_set_on_hour * 3600 + Valve_set_on_min * 60 + Valve_set_on_sec;
  Off_time_valve = Valve_set_off_hour * 3600 + Valve_set_off_min * 60 + Valve_set_off_sec;

  //  jsonWrite(configLiveJson, "Count_Down_valve_ON", String(On_time_valve));

  /*Serial.print("RECOUNT called on_time_valve: ");
    Serial.println(On_time_valve);
    Serial.print("RECOUNT called off_time_valve: ");
    Serial.println(Off_time_valve);
  */
}

void Alcorobot() {

  if (Mode == 0) {

    /*   if (!Is_valve_closed) {
      CloseSelectValve(); //закрыть
      Switch_PWM_on(false);

    }
*/
  } else if (Mode == 1) {
    Mode_1_Run();
  } else if (Mode == 2) {
    //если клапан закрыт
    if (Is_valve_closed) {
      OpenSelectValve();
      Switch_PWM_on(false);
    }
  }

}

void Switch_PWM_on(bool new_mode) {
  if (new_mode) {
    Recount_valve_time();

    if (!Is_close_by_NO_pwm) {
      OpenSelectValve();
    }

    Count_Down_valve_ON = On_time_valve;
    Count_Down_valve_OFF = Off_time_valve;
    Serial.println("New PWM mode is ON!");

    if (!Is_time_valve_ON) {
      Is_time_valve_ON = true;
    }
    if (Is_time_valve_OFF) {
      Is_time_valve_OFF = false;
    }
  } else {
    Serial.println("New PWM mode is OFF");
    //Count_Down_valve_ON = 0;
    //Count_Down_valve_OFF = 0;

    if (Is_time_valve_ON) {
      Is_time_valve_ON = false;
    }
    if (Is_time_valve_OFF) {
      Is_time_valve_OFF = false;
    }

  }

}

/* 1 - с ШИМ отобором (проверка на дельту верхнего градусника и проверка нижнего 
 * на разницу с установленной и отбор по ШИМу)
 */
void Mode_1_Run() {

  // if (!Is_tail_came) { //если только нет хвостов

  //если температура ниже больше установленной то переводим клапан в режим 0 - всегда закрыт
  if (Temp_bot - Temp_bot_set > 0.01) { // если хвосты пришли - то закрываем
    SetValveMode(0, false);
    TailLabelSwitcher(true);
/*
    CloseSelectValve();
    delay(300);
    if (!Is_tail_came) {
      TailLabelSwitcher(true);
    }
    */
  } else {
    //если разница между факт и устан температур больше дельты
    if (Temp_delta - Temp_delta_set > 0.05) {

      CloseSelectValve(); //закрываем
      if (!Is_close_by_NO_pwm) {
        Is_close_by_NO_pwm = true;
        DeltaLabelSwitcher(true);

      }

      //если разница в дельте не больше установленной но клапан закрыт по дельте
    } else if (Temp_delta - Temp_delta_set < -0.15 && Is_close_by_NO_pwm) {
      OpenSelectValve(); //открываем    
      Is_close_by_NO_pwm = false;
      DeltaLabelSwitcher(false);

    }

    if (!Is_close_by_NO_pwm && !Is_tail_came) {

      unsigned long past_millis = millis() - prev_millis;

      if (past_millis > 1000) {

        //если клапан должен быть открыт - щитаем время до закрытия
        if (Is_time_valve_ON) {
          Count_Down_valve_ON--;
          if (Count_Down_valve_ON > 0) { //если счетчик обратного отсчета не обнулен

            SoketData("Count_Down_valve_ON", String(Count_Down_valve_ON), "0"); // и новое и старое шлем

          } else { //если обнулен уже то переводим на клапан на закрытие

            SoketData("Count_Down_valve_ON", "0", "1");            
            Count_Down_valve_OFF = Off_time_valve;
            Is_time_valve_ON = false;
            Is_time_valve_OFF = true;
            CloseSelectValve(); //закрываем клапан
            On_time_valve_total_cicle++;
            SoketData("Cicles_on", String(On_time_valve_total_cicle), "-1");            
            
          }

        } else if (Is_time_valve_OFF) { // если клапан закрыт - щитаем время до открытия
          Count_Down_valve_OFF--;
          if (Count_Down_valve_OFF > 0) { //если счетчик обратного отсчета не обнулен

            SoketData("Count_Down_valve_OFF", String(Count_Down_valve_OFF), "0"); // и новое и старое шлем

          } else{ //если обнулен уже то переводим на клапан на открытие
            
            SoketData("Count_Down_valve_OFF", "0", "1");            
            Count_Down_valve_ON = On_time_valve;
            Is_time_valve_ON = true;
            Is_time_valve_OFF = false;
            OpenSelectValve();
         }

        }

        prev_millis = millis();

      }
    }
  }
  // }
}

