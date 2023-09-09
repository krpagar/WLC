//*********************************************************************
void checkForUpdates() 
{
  //String mac = getMAC();
  String mac = "SEMv5";
  String fwURL = String( fwUrlBase );
  fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );
  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );  
  int httpCode = httpClient.GET();

  //u8g2.clearBuffer();  
  //u8g2.setFont(u8g2_font_fub14_tf);
  //u8g2.setCursor(0, 15);
  //u8g2.print("HTTP: "); 
  //u8g2.print(httpCode);           
  //while ( u8g2.nextPage() );
  //delay(2000);
  //yield();
  
  if( httpCode == 200 ) 
  {    
    //digitalWrite(buzzer, HIGH);
    //delay(1000);
    //digitalWrite(buzzer, LOW);
      
    String newFWVersion = httpClient.getString();
    newVersion = newFWVersion.toInt();    

    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(10, 15);
    u8g2.print(" SW Ver"); 
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(00, 39);
    u8g2.print("CV: ");
    u8g2.print(FW_VERSION);
    u8g2.setCursor(00, 63);
    u8g2.print("LV: ");
    u8g2.print(newVersion);          
    while ( u8g2.nextPage() );
    delay(2000);
    yield();
    delay(1000); 

    if( newVersion > FW_VERSION ) 
    {      
      digitalWrite(buzzer, HIGH);
      delay(700);
      digitalWrite(buzzer, LOW);
      delay(300);
      yield();
      digitalWrite(buzzer, HIGH);
      delay(700);
      digitalWrite(buzzer, LOW);
      delay(300);
      yield();
      digitalWrite(buzzer, HIGH);
      delay(700);
      digitalWrite(buzzer, LOW);    
      
      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      
      ESPhttpUpdate.onStart(update_started);
      ESPhttpUpdate.onEnd(update_finished);
      ESPhttpUpdate.onProgress(update_progress);
      ESPhttpUpdate.onError(update_error);
      
      t_httpUpdate_return ret = ESPhttpUpdate.update(fwImageURL);

      switch(ret) 
      {
        case HTTP_UPDATE_FAILED:
          //Serial.printf(" HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          //Serial.println(" HTTP_UPDATE_NO_UPDATES");
          break;        
      }
    }
    else 
    {
      //Serial.println( "Already on latest version" );
      
    }
  }
  else 
  {
    //Serial.print( " failed, HTTP code " );
    //Serial.println( httpCode );
  }
  httpClient.end();
}
//******************************************************************************
void update_started() 
{
  //Serial.println("CALLBACK:  HTTP update process started");

  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(10, 15);
  u8g2.print("SW Ver."); 
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(15, 38);
  u8g2.print("Update");
  u8g2.setCursor(00, 60);
  u8g2.print("Started !");              
  while ( u8g2.nextPage() );
  delay(2500);
  yield();
      
  u8g2.clearBuffer();    
  u8g2.setFont(u8g2_font_open_iconic_www_6x_t);
  u8g2.drawGlyph(35, 63, 76);
  while ( u8g2.nextPage() );
  digitalWrite(buzzer, HIGH);
  delay(150);
  digitalWrite(buzzer, LOW);
  delay(150);
  digitalWrite(buzzer, HIGH);
  delay(150);
  digitalWrite(buzzer, LOW);
  delay(1500);
  yield();   
  //Play_Pirates();
  //delay(2500);
}

void update_finished() 
{
  //Serial.println("CALLBACK:  HTTP update process finished");
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(10, 15);
  u8g2.print("SW Ver."); 
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(15, 38);
  u8g2.print("Update");
  u8g2.setCursor(00, 60);
  u8g2.print("Finished!");              
  while ( u8g2.nextPage() );
  delay(1500);
}

void update_progress(unsigned int progress, unsigned int total) 
{
  //Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  char pro[10];     
  sprintf(pro,"%u",(progress/ (total/100)));            
  
  u8g2.clearBuffer();
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_freedoomr25_tn);
  u8g2.setCursor(32, 45); 
  
  if((progress/(total/100))<10)
  {
     u8g2.setCursor(42, 45);   
  }
  else if((progress/(total/100))>99)
  {
     u8g2.setCursor(27, 45);   
  }
  //u8g2.setCursor(35, 45);
  u8g2.print(pro);
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.println(" %");     
  while ( u8g2.nextPage() );
  delay(500);
}
//************************************************************************
void update_error(int err) 
{
  //Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
//**************************************************************************
