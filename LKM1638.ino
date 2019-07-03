#define Draw_LKM1638_int 1000 //период вывода на экран новых значений

//8-ми сегментный дисплей с ЛЕД-ами и кнопками LKM1638
void LKM1638_init(){

  SetBrightnessDisp1638(7);//включаем экран сначала на полную яркость

  //зажигаем все по очереди шоб видеть шо ничо не перегорело как в машине при включении зажигания :)))
 for(byte i = 0; i < 8; i++){
    tm.displayASCII(i, '8', true);
    tm.setLED(i, 1);
    delay(80);
  }
  for(byte i = 0; i < 8; i++){
    tm.displayASCII(i, ' ', false);
    tm.setLED(i, 0);
    delay(80);
  }
  for(int i = 7; i >= 0; i--){
    tm.displayASCII(i, '8', true);
    tm.setLED(i, 2);
    delay(80);
  }

  //обновляем на LKM1638 температуру
  ts.add(2, Draw_LKM1638_int, [&](void*) { // Запустим задачу 2 с интервалом Draw_LKM1638_int(1000)
    DrawTemperature();
     
  }, nullptr, true);

 
}


//рисуем темпрературу на LKM1638
void DrawTemperature(){

  RightTempToDisp(jsonRead(configLiveJson,"Temp_top"));
  LeftTempToDisp(jsonRead(configLiveJson,"Temp_bot"));
  
}

String Prev_str_temp_right = "";
//если рисовали сотню, то потом шоб не забыть стереть на экране
bool Is_hundred_draw_r = false;

//выводим дробное число (градусы) на правую половину
void RightTempToDisp(String str_temp){

  if(Prev_str_temp_right != str_temp){
    Prev_str_temp_right = str_temp;
    int point_index = str_temp.indexOf('.');
  
    //если разряд сотен градусов больше нуля то рисуем его
    if (int(str_temp[point_index - 3]) > 0)
    {
      tm.displayASCII(4, str_temp[point_index - 3]);
      Is_hundred_draw_r = true;
    }
    else if(Is_hundred_draw_r){
      Is_hundred_draw_r = false;
      tm.displayASCII(4, ' ');
    }
   
    //рисуем поразрядно значения в первом сегменте - СПРАВА
    tm.displayASCII(5, str_temp[point_index - 2]);
    tm.displayASCII(6, str_temp[point_index - 1], true);
    tm.displayASCII(7, str_temp[point_index + 1]);
  }
}

String Prev_str_temp_left = "";
//если рисовали сотню, то потом шоб не забыть стереть на экране
bool Is_hundred_draw_l = false;
//на левые 4 сегмента
void LeftTempToDisp (String str_temp){

  if(Prev_str_temp_left != str_temp){
    Prev_str_temp_left = str_temp;
    int point_index = str_temp.indexOf('.');
    if (int(str_temp[point_index - 3]) > 0)
    {
      tm.displayASCII(0, str_temp[point_index - 3]);
      Is_hundred_draw_l = true;
    }
    else if(Is_hundred_draw_l){
      Is_hundred_draw_l = false;
      tm.displayASCII(0, ' ');
    }

    //слева
    tm.displayASCII(1, str_temp[point_index - 2]);
    tm.displayASCII(2, str_temp[point_index - 1], true);
    tm.displayASCII(3, str_temp[point_index + 1]);
  }
}

void BrightnessDisp1638Init(){

    String brDisp1638_saved = jsonRead(configSetup, "Disp1638br");
    jsonWrite(configLiveJson, "Disp1638br", brDisp1638_saved);        
    Serial.print("brDisp1638_saved is: ");
    Serial.println(brDisp1638_saved);
    SetBrightnessDisp1638(brDisp1638_saved.toInt());//применяем сохранненое значение яркости
}

//Яркость экрана
void SetBrightnessDisp1638(uint8_t br){
     tm.sendCommand(0x80 | (true ? 8 : 0) | br);
}

//вторые 4 леда отдаем под режимы работы
void SetLedMode(byte num_led){
 for(byte i = 4; i < 8; i++){
    tm.setLED(i, 0);
    delay(10);
  }
  tm.setLED(num_led + 4, 1);
}

// 3 и 4 лед отдаем под состояние отбора
void SetLedIsTailCame(bool yes){
  if(yes){
    tm.setLED(3, 1);
  }else{
    tm.setLED(3, 2);    
  }
}

void SetLedIsDelta(bool yes){
  if(yes){
    tm.setLED(2, 1);
  }else{
    tm.setLED(2, 2);    
  }
}

//мигаем крайним справа диодом
//красным - если ошибка в термометрах is_green = true
void BlinkInfoLed(bool is_green){
 if(is_green){
    tm.setLED(7, 1);
  }else{
    tm.setLED(7, 2);    
  }
  delay(100);
  tm.setLED(7, 0);    
}



