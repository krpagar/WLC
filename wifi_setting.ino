//******************************************************************************
void normal_bootup()
{    
    loadCredentials();
    wifi_init();         
    wifi_connect(); 
    
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid, softAP_password);
    dnsServer.start(DNS_PORT, "*", apIP);
    MDNS.begin(host); 
    httpServer.on("/", handleRoot);
    httpServer.on("/wifi", handleWifi);
    httpServer.on("/wifisave", handleWifiSave);
    httpServer.on("/wifisetting", energy_cycle);
    httpServer.on("/wifikwhreset", clear_data);
    httpServer.on("/generate_204", handleRoot);       //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    httpServer.on("/fwlink", handleRoot);             //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    httpServer.on("/SEM", handle_OnConnect);    
    httpServer.onNotFound(handleNotFound);
    httpUpdater.setup(&httpServer);        
    httpServer.begin();        
    MDNS.addService("http", "tcp", 80);          
    Serial.printf("\nhttp://%s.local/update \n OR ", host);        
    Serial.print(WiFi.localIP());
    Serial.println("/update");
    delay(500);    
        
    if (WiFi.status() == WL_CONNECTED) 
    {
        if(WiFi.localIP().isSet())
        {
            for(unsigned char wt=0;wt<40;wt++)
            {
              httpServer.handleClient();
              MDNS.update();
              delay(50);
              yield();
            } 
        }         
    }    
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fub17_tf);   
    u8g2.setCursor(00, 46);
    u8g2.print("GSM Init...");
    while ( u8g2.nextPage() );
    delay(500);

    if (WiFi.status() == WL_CONNECTED) 
    {
        if(WiFi.localIP().isSet())
        {
           bool ret = Ping.ping("www.google.com", 2);
           if(ret = 1)
           {          
              Cayenne.begin(CayenneUser, CayennePass, CayenneID_Wifi);                       
              yield(); 
              first_time_wifi=0;
              checkForUpdates();
           }
        }
    }
    dht.setup(0, DHTesp::DHT11);                //Change for DHT11 or DHT22
    delay(dht.getMinimumSamplingPeriod());
    delay(100); 
   
    ATMSerial.begin(9600);
    delay(100);    
    unsigned short s_status = eic.GetSysStatus();
    Serial.print("s_status=");
    Serial.println(s_status);     
    eic.InitEnergyIC();
    delay(100);  
    
    energy_temp=EEPROM_get(20);      
    delay(10);
    last_enrgy=EEPROM_get(30);
    delay(10);    
    second_day_ergy=EEPROM_get(40);             // stored energy
    delay(10);
    first_day_ergy=EEPROM_get(10);              // stored energy
    delay(10);   
        
    if (!driver.init())
    Serial.println("init failed");
    delay(100);     

    if (WiFi.status() == WL_CONNECTED) 
    {
      if(WiFi.localIP().isSet())
        {
          for(char wt=0;wt<40;wt++)
          {
            httpServer.handleClient();
            MDNS.update();
            delay(25);
            rssi = WiFi.RSSI();
            yield();
          }
        }
    }

    
/*    GSM_init();    
    GSM_time();    

    SerialAT.println("AT+CLTS=1");
    delay(1000);
    yield(); 
    SerialAT.println("AT&W");
    delay(1000); 
*/
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fub20_tf);   
    u8g2.setCursor(4, 44);
    u8g2.print("READY !");       
    while ( u8g2.nextPage() );
    delay(200); 

    digitalWrite(buzzer, HIGH);
    delay(10);
    digitalWrite(buzzer, LOW);               
}
//********************************************************************
void wifi_init()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);  
  uint8_t w = 0;
  delay(500);   
  while (WiFi.waitForConnectResult() != WL_CONNECTED && w++ < 8) 
  {    
    u8g2.clearBuffer();
    u8g2.firstPage();    
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(40, 15); 
    u8g2.print("Wi-Fi");
    u8g2.setFont(u8g2_font_fub20_tf);
    u8g2.setCursor(9, 60);
    u8g2.println("Absent !");    
    while ( u8g2.nextPage() );
    delay(1200);   
    switch_input();   
  }
  //serial.println(WiFi.localIP());
}
//*************************************************************
void wifi_connect()
{  
  if (WiFi.status() == WL_CONNECTED) 
  {    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_open_iconic_all_6x_t);
    u8g2.drawGlyph(35, 64, 120);
    while ( u8g2.nextPage() );
    delay(1000);
    rssi = WiFi.RSSI();

    GSM_RSSI_Bars();
/*
    long rssi = WiFi.RSSI();    
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fub35_tf);
    u8g2.setCursor(24, 63);
    u8g2.print(rssi); 
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(26, 15);
    u8g2.print("WiFi RSSI");          
    while ( u8g2.nextPage() );
    delay(2000);
    yield();  */
 /*   
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(32, 15); 
    u8g2.print("Wi-Fi ");
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(0, 38);
    //u8g2.print(">");
    u8g2.print(ssid);
    //u8g2.print("<");
    u8g2.setFont(u8g2_font_fub11_tf);
    u8g2.setCursor(0, 62);
    u8g2.print(WiFi.localIP());
    //u8g2.setCursor(0, 33);      
    while ( u8g2.nextPage() );
    delay(500); 
    yield();   */     
  }
  //else normal_bootup();
  //TelnetServer.begin();   //Necesary to make Arduino Software autodetect OTA device 
  else
  {
      GSM_RSSI_Bars();    
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_fub35_tf);
      u8g2.setCursor(26, 63);
      u8g2.print(GSM_RSSI); 
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.setCursor(20, 15);
      u8g2.print("GSM RSSI");          
      while ( u8g2.nextPage() );
      delay(2000);
      yield();  
  }
}
//*********************************************************
/** Is this an IP? */
boolean isIp(String str) 
{
  for (size_t i = 0; i < str.length(); i++) 
  {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) 
    {
      return false;
    }
  }
  return true;
}
//******************************************************
/** IP to String? */
String toStringIp(IPAddress ip) 
{
  String res = "";
  for (int i = 0; i < 3; i++) 
  {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
