/** Handle root or redirect to captive portal */
void handleRoot()
{
  if (captivePortal())
  { // If caprive portal redirect instead of displaying the page.
    return;
  }
 
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");

  String Page;
  Page += F(
            "<!DOCTYPE html><html lang='en'> <head>"
            "<meta name='viewport' content='width=device-width'>"
            "<style> body { background-color: #b0967d; font-family: Antique Olive; text-align: center; Color: #330000; }</style>"
            "<title>CaptivePortal</title></head><body>"
            "<h1> Welcome to Smart-MEM v4.1 Pro</h1>"
            "<h2> Thanks for selecting Smart-MEM </h2>");
  if (httpServer.client().localIP() == apIP)
  {
    Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  }
  else
  {
    Page += String(F("<p>You are connected through the wifi network: ")) + WiFi.SSID() + F("</p>");
  }
  Page += F(
            "<p><br> <h3> Please click for <a href='/wifi'> Configuration </a>  OR  <a href='/update'> Firmware update </a>  OR  <a href='/SEM'> Metering Data. </a></h3> </br> </p>"
            "</body></html>");

  httpServer.send(200, "text/html", Page);
}
//****************************************************************************************************************************
boolean captivePortal()
{
  if (!isIp(httpServer.hostHeader()) && httpServer.hostHeader() != (String(myHostname) + ".local"))
  {
    //Serial.println("Request redirected to captive portal");
    httpServer.sendHeader("Location", String("http://") + toStringIp(httpServer.client().localIP()), true);
    httpServer.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    httpServer.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}
//***************************************************************************************************************************8
void handleWifi()
{
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");

  String Page;
  Page += F(
            "<!DOCTYPE html><html lang='en'><head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>CaptivePortal</title></head><body>"
            "<h1>Wifi config</h1>");
  if (httpServer.client().localIP() == apIP)
  {
    //Page += String(F("<p>You are connected through the soft AP: ")) + softAP_ssid + F("</p>");
  }
  else
  {
    //Page += String(F("<p>You are connected through the wifi network: ")) + ssid + F("</p>");
  }
  Page +=
    String(F(
             "<style>"
             "body "
             "{"
             "   background-color: #6091db;"
             "   text-align: center;"
             "   color: white;"
             "   font-family: Calibri, Helvetica, sans-serif;"
             "}" 
             "</td></tr>"));
  Page += F(  

            "input[type=submit] "
            "{"
            "  background-color: #ab8259;"
            "  color: black;"
            "  padding: 6px 20px;"
            "  border: none;"
            "  border-radius: 5px;"
            "  cursor: pointer;"
            "}            "
            " input[type=submit]:hover "
            "{"
            "  background-color: #c97b89;"
            "}            "
            "</style>   "
            //"<img src='avatar.png' alt='Avatar' style='width:200px'> "

            "<br /><form method='POST' action='wifisave'> <h2>Connect to WiFi Network </h2>"
            "<input type='text' placeholder='network' name='n'/>"
            "<br /><input type='password' placeholder='password' name='p'/>"
            "<br /><input type='submit' value='Connect'/></form>  "

            "<br /><form method='POST' action='wifisetting'> <h2>  Energy Metering Cycle Start Date </h2>"
            "<input type='text' placeholder='date' name='d'/>"
            "<br /><input type='submit' value='Submit'/></form>    "

            "<br /><form method='POST' action='wifikwhreset'> <h2>  Reset kWH ? </h2>"
            "<input type='submit' value='Reset'/> </form>"

            "<br /> "
            "<br />"
            "<input type='checkbox' id='vehicle1' name='vehicle1' value='Bike'>"
            "<label for='vehicle1'> Temperature & Humidity Logging </label><br>"

            "<input type='checkbox' id='vehicle2' name='vehicle2' value='Bike'>"
            "<label for='vehicle2'> Data logging over Cellular/ GSM </label><br>"

            "<br />"
            "<br />"
            "<p>You may want to <a href='/'>return to the home page</a>.</p>" );
   Page += F(
            "<p>Energy Meter Data <a href='/SEM'> Here </a></p>"
            "</body></html>");

  httpServer.send(200, "text/html", Page);
  httpServer.client().stop();                     // Stop is needed because we sent no content length
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave()
{
  //serial.println("wifi save");
  httpServer.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  httpServer.arg("p").toCharArray(password, sizeof(password) - 1);
  httpServer.sendHeader("Location", "wifi", true);
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");
  httpServer.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  httpServer.client().stop(); // Stop is needed because we sent no content length

  saveCredentials();

  connect = strlen(ssid) > 0;               // Request WLAN connect with new credentials if there is a SSID

  loadCredentials();
  delay(10);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub20_tf);
  u8g2.setCursor(0, 30);
  u8g2.print(ssid);
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(0, 60);
  u8g2.print("connect..");
  while ( u8g2.nextPage() );
  delay(500);
  yield();
  //ESP.restart();
  //switch_input();
}

void handleNotFound()
{
  if (captivePortal())
  { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += httpServer.uri();
  message += F("\nMethod: ");
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += httpServer.args();
  message += F("\n");

  for (uint8_t i = 0; i < httpServer.args(); i++) {
    message += String(F(" ")) + httpServer.argName(i) + F(": ") + httpServer.arg(i) + F("\n");
  }
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");
  httpServer.send(404, "text/plain", message);
}
//*************************************************************************************
void energy_cycle()
{  
  
  httpServer.arg("d").toCharArray(date, sizeof(date) - 1);  
  httpServer.sendHeader("Location", "wifi", true);
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");
  httpServer.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  httpServer.client().stop(); // Stop is needed because we sent no content length
 
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(18, 15);
  u8g2.print("Bill Date"); 
  u8g2.setFont(u8g2_font_fub35_tf);
  u8g2.setCursor(35, 63);
  u8g2.print(date);          
  while ( u8g2.nextPage() );
  delay(2500);
  yield();
}
//**************************************************************************************
void clear_data()
{
  httpServer.sendHeader("Location", "wifi", true);
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");
  httpServer.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  httpServer.client().stop(); // Stop is needed because we sent no content length
  
  for(char i=0;i<50;i++)
  {
    EEPROM_put(i, 0);
    delay(150);
  }
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(00, 15);
  u8g2.print("SEMS Memory"); 
  u8g2.setFont(u8g2_font_fub25_tf);
  u8g2.setCursor(10, 62);
  u8g2.print("CLEAR !!");          
  while ( u8g2.nextPage() );
  delay(2500);
  yield();
  
  ESP.restart();
}
//**************************************************

void handle_OnConnect() 
{

  //Temperature = dht.readTemperature();               // Gets the values of the temperature
  //Humidity = dht.readHumidity();                    // Gets the values of the humidity 
  httpServer.send(200, "text/html", SendHTML(power_temp)); 
}

void handle_NotFound()
{
  httpServer.send(404, "text/plain", "Not found");
}

String SendHTML(int power_temp)
{
  httpServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  httpServer.sendHeader("Pragma", "no-cache");
  httpServer.sendHeader("Expires", "-1");

  String ptr;
  ptr += F(   
            "<html>"
            "<style>"
            "table, th, td" 
            "{"
              "border: 2px solid black;"
              "border-collapse: collapse;"
              "font-family: Calibri;"
              "font-size: 20px;"
              "text-align: center;"
              "background: #b89fcc;"              
            "}"
            "th, td {"
              "padding: 10px;"
              "text-align: center;"
            "}"
            "table.center {"
            "margin-left: auto;" 
            "margin-right: auto;}"
                       
            "</style>"
            "<body>"
            "<meta http-equiv='refresh' content='3'>"
            "<div class='card'>"
            "<h1>Smart Energy Meter Dashboard V1.2</h1><br>" 
            "<table style='width:50%'>"            
            //"<caption>Smart Energy Data</caption>"            
            "<tr>"  
              "<th>Parameter</th>"
              "<th>Value</th>"
            "</tr>"            
            "<tr>"
              "<td>Voltage</td><td>");
            ptr +=(int)volt_temp;
            ptr +="</td></tr><tr><td>Current</td><td>";
            ptr +=(float)current_temp;
            ptr +="</td></tr><tr><td>Power</td><td>";
            ptr +=(int)power_temp;
            ptr +="</td></tr><tr><td>Energy (WHr)</td><td>";
            ptr +=(int)kwhr_temp;
            ptr +="</td></tr><tr><td>Frequency</td><td>";
            ptr +=(float)freq_temp;
            ptr +="</td></tr><tr><td>RF</td><td>";
            ptr +=(long int)remote_door_sensor_code;
            ptr +="</td></tr><tr><td>UpTime</td><td>";
            ptr +=(int)hours;       
            ptr +=":";
            ptr +=(int)minutes;      //u8g2.print(WiFi.localIP());
            ptr +="</td></tr><tr><td>Local SSID</td><td>";
            ptr += WiFi.SSID();
            ptr +="</td></tr><tr><td>Remote Vbatt</td><td>";
            ptr += (float)vbatt_1;
            ptr +="</td></tr><tr><td>Remote WLC</td><td>";
            ptr += (int)wlc_1;
             ptr +="</td></tr><tr><td>Remote Vin</td><td>";
            ptr += (float)vin_1_temp;
            ptr +="</td></tr><tr><td>Remote Temp</td><td>";
            ptr += (float)temp_1;
            ptr +="</td></tr></table>";            
            //ptr +="<p><br> <h3> <a href='/update'> Firmware update </a> </h3> </br> </p>";
            ptr +="<p>You may want to <a href='/'>return to the home page</a>.</p>" ;
            ptr +="</div>\n";
            ptr +="</body>\n";
            ptr +="</html>\n";
            return ptr;   
}
//**********************************************************************************
