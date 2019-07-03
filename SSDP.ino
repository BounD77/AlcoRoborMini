void SSDP_init(void) {
  String chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  // SSDP дескриптор
 /* HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  */
   // --------------------Получаем SSDP со страницы
  HTTP.on("/ssdp", HTTP_GET, []() {
    String ssdp = HTTP.arg("ssdp");
  //configLiveJson=jsonWrite(configLiveJson, "SSDP", ssdp);
  configLiveJson=jsonWrite(configSetup, "SSDP", ssdp);
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  saveConfig();                 // Функция сохранения данных во Flash
  HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  SSDP.setSerialNumber(chipID);
  SSDP.setURL("/");
  SSDP.setModelName("tech");
  SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, "SSDP"));
  SSDP.setManufacturer("Misa Off");
  SSDP.setManufacturerURL("http://misa.asim");
  SSDP.begin();
}
