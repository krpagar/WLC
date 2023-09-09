/** Load WLAN credentials from EEPROM */
void loadCredentials() 
{
  EEPROM.begin(512);
  delay(250);
  EEPROM.get(50, ssid);
  EEPROM.get(50 + sizeof(ssid), password);
  char ok[2 + 1];
  EEPROM.get(50 + sizeof(ssid) + sizeof(password), ok);
  delay(250);
   
  if (String(ok) != String("OK")) 
  {
    ssid[0] = 0;
    password[0] = 0;
  } 

  delay(300);
 // u8g2.clearBuffer();  
 // u8g2.setFont(u8g2_font_fub14_tf);
 // u8g2.setCursor(12, 31);
 // u8g2.print(ssid); 
 // u8g2.setFont(u8g2_font_fub14_tf);
 // u8g2.setCursor(05, 63);
 // u8g2.print(password);          
 // while ( u8g2.nextPage() );
 // delay(1500);
}
//****************************************************************************
void saveCredentials() 
{
  EEPROM.begin(512);
  delay(250);
    
  EEPROM.put(50, ssid);
  delay(300);
  EEPROM.commit();  
  delay(250);
  
  EEPROM.put(50 + sizeof(ssid), password);
  delay(300);
  EEPROM.commit();  
  delay(200);
  
  char ok[2 + 1] = "OK";
  EEPROM.put(50 + sizeof(ssid) + sizeof(password), ok);
  delay(300);
  EEPROM.commit();  
  delay(100);

  int config_wifi=84;
  EEPROM_put(100, config_wifi);
  EEPROM.put(100, config_wifi);
  delay(300);
  EEPROM.commit();
  delay(300);
  //wifi_sta_connect=1;
  
  delay(100);
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub20_tf);
  u8g2.setCursor(0, 25);
  u8g2.print(ssid); 
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(0, 60);
  u8g2.print(password);          
  while ( u8g2.nextPage() );
  delay(2000);
  yield();

  ESP.restart();
}
//***************************************************
