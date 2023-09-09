/*************************************************** 
 *  SmartEM (Smart Energy Meter v6.0)
 *  Data: 15th August, 2023 @2156
 *  SEM V1.2.014
 *  484656 bytes (46%)
 *  35444 bytes (43%)
 ****************************************************/
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <Wire.h> 
//#include <ESP_SSD1306.h>          
#include <U8g2lib.h>
#include <RH_ASK.h>
#include <energyic_UART.h>
#include <CayenneMQTTESP8266.h>
#include "DHTesp.h"
#include <ZEeprom.h>
#include <ESP8266Ping.h>
//#include "pitches.h" //add Equivalent frequency for musical note
//#include "themes.h" //add Note vale and duration
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

const char* fwUrlBase = "https://iothinks.in/image/"; // dont use https.. http will work only!! //const char* fwUrlBase = "http://192.168.0.1/fota/";
const int FW_VERSION = 5000;                       // updated on 01/09/2022

#define EEPROM_ADDRESS  0x50
ZEeprom * eeprom;
byte inputBytes[4] = { 0 };

const long utcOffsetInSeconds = 19800;                        
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
/*************************** FONA Pins ***************************************/
#define FONA_APN       "airtelgprs"
#define FONA_USERNAME  ""
#define FONA_PASSWORD  ""

#define TINY_GSM_MODEM_SIM800
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 19200
#define TINY_GSM_YIELD() { delay(2); }    // changed for trial purpose

int mqtt_port = 1883;
const char* mqtt_server = "mqtt.mydevices.com";
const char* CayenneUser = "3d6bf0d0-cf57-11ea-b767-3f1a8f1211ba";
const char* CayennePass = "63e09c0d9e4aaf65f4494b5d95e56ef8045a02ea";
const char* CayenneID = "9e02b240-81c5-11ec-9f5b-45181495093e";              // Rameshwar home
//const char* CayenneID_Wifi = "cd672980-44ba-11ee-9ab8-d511caccfe8c";       // Rameshwar Wifi New
const char* CayenneID_Wifi = "bd163990-4f37-11ee-8485-5b7d3ef089d0";    //Taken on 9thSep for WLC testing
//365798c0-4c92-11ed-bf0a-bb4ba43bd3f6
//const char* CayenneID_Wifi = "365798c0-4c92-11ed-bf0a-bb4ba43bd3f6"; //Pune Home 
//const char* CayenneID_Wifi = "9e02b240-81c5-11ec-9f5b-45181495093e";     // Rameshwar home//
//"f442bf70-ea80-11ea-b767-3f1a8f1211ba"; changed on 30th April, 2021
//const char* CayenneID = "f442bf70-ea80-11ea-b767-3f1a8f1211ba";

//const char* CayenneID_Wifi = "67751160-b479-11eb-883c-638d8ce4c23d";
//const char* CayenneUser = "3d6bf0d0-cf57-11ea-b767-3f1a8f1211ba";
//const char* CayennePass = "63e09c0d9e4aaf65f4494b5d95e56ef8045a02ea";
//const char* CayenneID = "8a309f30-7cb1-11eb-b767-3f1a8f1211ba";   //

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#define SerialAT Serial
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
/************************* WiFi Access Point *********************************/
//#define AIO_SERVER      "io.adafruit.com"
//#define AIO_SERVERPORT  1883
//#define AIO_USERNAME    "krpagar"
//#define AIO_KEY         "0570984dabe39fabd53bbd196a29eb19b345ba98"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  

SoftwareSerial ATMSerial(13, 2);                        
ATM90E26_UART eic(&ATMSerial);

DHTesp dht;
RH_ASK driver(600, 14, 12, -1); //rx,tx 

uint8_t txfailures = 0;
#define MAX_SRV_CLIENTS 3
#define MAXTXFAILURES 3

#ifndef APSSID
#define APSSID "SmartEM"
#define APPSK  ""
#endif

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

char date[10]="";
char ssid[18] = "";
char password[18] = "";
const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;
const char *myHostname = "SmartEM";
const char *host = "SmartEM";
const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer server(80);
IPAddress apIP(192, 168, 4, 1);
//IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);
boolean connect;
unsigned long lastConnectTry = 0;
unsigned int status = WL_IDLE_STATUS;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

int8_t rssi_gsm=0;
int8_t send_data=0;
int energy_acc=0;
int Ergy=0;
uint32_t x=0;
int humidity_temp;
int temperature_temp;
char data_send_cloud=0;
char store_enrgy[6];
int32_t current_enrgy=0;
int max_power=0;
int32_t energy_temp=0;
float energy_temp_float=0;
int max_power_temp=0;
int volt_temp=0;
int power_temp=0;
int32_t data_buf=0;
int RF_value=0;
char it;
int energy_temp_trial;  
int hr;
int mint;
int hr_buf;
int mint_buf;
int hr_start;
int mint_start;
float pf_temp=0;
float freq_temp=0;
int power_VA;    
int Irms_temp;
float current_temp=0;
int cloud_update;   
void MQTT_Cloud(); 
int cloud_cnt;
void smart_delay(int delay_ms);
int mul_factor=0;
int ERGY_write=5;
int ERGY_send=0;
int EEPROM_wrote=0;
int energy_temp_check=0;
int32_t last_enrgy=0;
int diff_enrgy=0;
int todays_energy_disp=0;
int second_day_ergy=0,first_day_ergy=0;
unsigned int Lsense_temp=0;         
int Tsense_temp=0;                          
unsigned int Vsense_temp=0;
int only_once_here=0;
int no_wifi=0;
int MQTT_sent_counter=0;
const int buttonPin = 16, buzzer = 15;     
int buttonState = 0;         
int config_wifi=0;
void FONAconnect();
void MQTT_connect();
int32_t NRGY_send=0;
long rssi;
int energy_eeprom=0;
int RF_buffer_array[25];
uint8_t buf[10];
uint8_t buflen = sizeof(buf);
long value_read;
int first_time_wifi=0;
unsigned char all_display=0;
int RF_data_recv=0;
signed int GSM_RSSI;
String Present;
String Absent;
unsigned long previousMillis = 0;
const long interval = 14400000;  //4hrs= 14400sec  
int modem_restart=0; 
int modem_reset_try=0;
  
//unsigned long currentMillis=0;

#define LED_PIN 2

int ledStatus = LOW;
long lastReconnectAttempt = 0;
long sendoutTime;
int volt_send=0,power_send=0;
unsigned char rf_raw_data[15];
float rf_data_store;
float Lsense_ft, Vsense_ft;
long int rf_data;
float Temp_float=0;
float vbatt_send_float=0;
uint16_t Tsense; 
uint16_t vbatt_send;
uint16_t Udist;
uint16_t Ldist; 
uint16_t rf_address;
float vbatt;
uint16_t kwhr_temp;
String ipadd;
uint8_t Ary[6];
uint16_t hr_GSM;
uint8_t Ary_date[9];

uint8_t Ary_mint[2];
int last_enrgy_start=0;
int newVersion=0;
int GSM_present=0;
int water_level=0;
unsigned char counter=0;

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L) 
/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY) 

int days;
int hours;
int minutes;
int seconds;
int minutes_buf=0; 

//#define TINY_GSM_RX_BUFFER   1024               // Set RX buffer to 1Kb
#define VIRTUAL_CHANNEL 20
unsigned long remote_door_sensor_code=0;
char one_time_pmax=0;
unsigned int buffer_hold=0;
float vbatt_data,wlc_data;
unsigned int vbatt_data_buf,wlc_data_buf;
unsigned long data_out_1;

float temp_1;
unsigned char wlc_1;
unsigned char vin_1;
float vin_1_temp=0;
float vbatt_1;

/*************************** Sketch Code ************************************/
void setup() 
{
  Serial.begin(9600);    
  
  pinMode(buttonPin, INPUT);
  pinMode(buzzer, OUTPUT);
  
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.firstPage();  
  u8g2.setFontMode(2);  
  u8g2.setDrawColor(1);  
  u8g2.drawBox(0, 26, 128, 63);      
  u8g2.setFont(u8g2_font_fub30_tf);  
  u8g2.setDrawColor(2);
  u8g2.drawStr(04,62,"KEMS");  
  u8g2.setFont(u8g2_font_fub14_tf);  
  u8g2.drawStr(00,15,"2023   09SEP");
  while ( u8g2.nextPage() );
  delay(500);
  yield(); 
    
  eeprom= new ZEeprom();
  delay(20);
  eeprom->begin(Wire,AT24Cxx_BASE_ADDR,AT24C02);
  delay(200);
  EEPROM.begin(512);
  delay(200);

  //i2c_scanner();
  /*for(char i=0;i<50;i++)
  {
    EEPROM_put(i, 0);
    delay(150);
  }
*/
 
  normal_bootup();   
  all_display=3;  

  mySwitch.enableReceive(14);       // Receiver input on interrupt 0 (D2) 
}
//********************************************************
void rf_door_sensor_check()
{
  remote_door_sensor_code=0;
  if (mySwitch.available()) 
  {
    remote_door_sensor_code = mySwitch.getReceivedValue();
    rf_data = remote_door_sensor_code;
        
    Serial.println(remote_door_sensor_code);   
    mySwitch.resetAvailable(); 
      
    if(remote_door_sensor_code == 16041553)      // decoded code SONOFF
    {
        u8g2.clearBuffer();   
        u8g2.setFont(u8g2_font_fub14_tf);
        u8g2.setCursor(10, 34);       
        u8g2.print("Son-Off");    
        u8g2.setCursor(0, 54);       
        u8g2.print("Door Opened !");   
        while ( u8g2.nextPage() );
        yield();        
        smart_delay(15);
        mySwitch.resetAvailable();

        digitalWrite(buzzer, HIGH);
        delay(30);
        digitalWrite(buzzer, LOW);
        //delay(30);
        //digitalWrite(buzzer, HIGH);
        //delay(60);
        //digitalWrite(buzzer, LOW);
        //smart_delay(2);       
        yield();
    }
    if(remote_door_sensor_code == 2538345)      // decoded code
    {
        u8g2.clearBuffer();   
        u8g2.setFont(u8g2_font_fub14_tf);   
        u8g2.setCursor(0, 44);       
        u8g2.print("Door Opened !");   
        while ( u8g2.nextPage() );
        yield();        
        smart_delay(15);
        mySwitch.resetAvailable();

        digitalWrite(buzzer, HIGH);
        delay(30);
        digitalWrite(buzzer, LOW);
        //delay(30);
        //digitalWrite(buzzer, HIGH);
        //delay(60);
        //digitalWrite(buzzer, LOW);
        //smart_delay(2);       
        yield();
    }
    if(remote_door_sensor_code == 14763818)      // decoded code
    {
        u8g2.clearBuffer();   
        u8g2.setFont(u8g2_font_fub14_tf);   
        u8g2.setCursor(0, 44);       
        u8g2.print("Motion !!");   
        while ( u8g2.nextPage() );
        yield();        
        smart_delay(15);
        mySwitch.resetAvailable();

        digitalWrite(buzzer, HIGH);
        delay(30);
        digitalWrite(buzzer, LOW);
        //delay(30);
        //digitalWrite(buzzer, HIGH);
        //delay(60);
        //digitalWrite(buzzer, LOW);
        //smart_delay(2);       
        yield();
    }    
    else
    {

        data_out_1=remote_door_sensor_code;
        
        temp_1=(data_out_1>>24);  
        temp_1=(float)(temp_1*2)/10;
        wlc_1=((data_out_1)>>16);
        
        vin_1=((remote_door_sensor_code)>>8);
        float vin_1_float=(float)(vin_1*2)/100; 
        vin_1_temp=vin_1_float;
        
        vbatt_1=(data_out_1 & 0x000000FF);
        vbatt_1=(float)(vbatt_1*2)/100;
        
        u8g2.clearBuffer();   
        u8g2.setFont(u8g2_font_fub14_tf);

        u8g2.setCursor(0, 15); 
        u8g2.println(">RF Node<");    //Serial.println(remote_door_sensor_code);  14763818
        //u8g2.println(temp_1);    //Serial.println(remote_door_sensor_code);  14763818
        //u8g2.println(" c");

        u8g2.setFont(u8g2_font_fub11_tf);        
        u8g2.setCursor(0, 28); 
        u8g2.println("temp: ");    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(temp_1,1);    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(" c");
        
        u8g2.setCursor(0, 40); 
        u8g2.println("wlc  : ");    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(wlc_1);    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(" cm");

        u8g2.setCursor(0, 52); 
        u8g2.println("vin  : ");    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(vin_1_float,2);    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(" v");

        u8g2.setCursor(0, 64); 
        u8g2.println("vbat: ");    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(vbatt_1,2);    //Serial.println(remote_door_sensor_code);  14763818
        u8g2.println(" v");  
        
 
        while ( u8g2.nextPage() );
        yield(); 
        //digitalWrite(buzzer, HIGH);
        //delay(30);
        //digitalWrite(buzzer, LOW);
        //delay(30);
        //digitalWrite(buzzer, HIGH);
        //delay(60);
        //digitalWrite(buzzer, LOW);       
        smart_delay(18);
        //smart_delay(20);
        mySwitch.resetAvailable();  
    }
  }  
}
//********************************************************
void loop() 
{    
  EMS_Display();                    //Metering parameters 
  wifi_status_check();  
  time(millis() / 1000);
  rf_door_sensor_check();
  //AS3935_loop();  
}
//*******************************************************
void time(long val)
{  
  days = elapsedDays(val);
  hours = numberOfHours(val);
  minutes = numberOfMinutes(val);
  seconds = numberOfSeconds(val); 
  minutes_buf=minutes;
}
//***********************************************************
void wifi_status_check()
{
  httpServer.handleClient();
  dnsServer.processNextRequest();
      
  if (WiFi.status()==WL_CONNECTED) 
  {       
    if(WiFi.localIP().isSet())
    {
      //httpServer.handleClient();
      //dnsServer.processNextRequest();
      MDNS.update();              
      rssi = WiFi.RSSI();
      if(no_wifi==1)
      first_time_wifi=1;        
      no_wifi=0; 
      //rf_door_sensor_check();
    }               
  }
  else no_wifi=1;
}
//*************************************************************
void EMS_Display() 
{                
    counter++;
    rf_door_sensor_check();       
    for(char jk=0;jk<15;jk++)
    {
      //Remote_data_process();
      wifi_status_check();           
      delay(4);    
    }
    rf_door_sensor_check();           
    ATM_EMS_measure();    
    //GSM_time();            
    if((Ary_date[0]==2) && (Ary_date[1]==2))
    {
      //GSM_time_display();
    }    
    else 
    {
      smart_delay(2);
      //GSM_time();
    }
    //AS3935_loop();
    if((Ary_date[0]==2) && (Ary_date[1]==2))
    {
      //GSM_time_display();
    }

    power_display();
    //WLC();
    if((power_temp>1000) && one_time_pmax==0)
    {
      digitalWrite(buzzer, HIGH);
      delay(80);
      digitalWrite(buzzer, LOW);
      //delay(30);
      //digitalWrite(buzzer, HIGH);
      //delay(60);
      //digitalWrite(buzzer, LOW);
      one_time_pmax=1;                
    }
    if((power_temp<100))
    {      
      one_time_pmax=0;                
    } 
    
    volt_temp=eic.GetLineVoltage();
    if(volt_temp>30 && counter>20)
    {        
        counter=0;        
        //GSM_time();
        time_display();
        rf_door_sensor_check();
        volt_display();
        current_display();              
        rf_door_sensor_check();
        ATM_EMS_measure();   
             
        max_power_display();                
        rf_door_sensor_check();
        energy_display();
        //thermometer_display();        
        if(ERGY_send==1)
        {                
          today_energy_display(); 
          //cycle_energy_display();  
        }         
        time_display();
        rf_door_sensor_check();
        //freq_pf_display();
        //GSM_cloud();
        MQTT_Wifi();
        checkForUpdates();                   
        yield();               
    }     
}
//*************************************************************
void GSM_time()
{
  uint8_t i = 0, j = 0, k=0,l=0,time_buf[6];
  uint8_t hr1=0,hr2=0,hr3=0;
  
  String gsmTime = modem.getGSMDateTime(DATE_TIME);
  String gsmDate = modem.getGSMDateTime(DATE_DATE);

  while (k < 6)
  {    
    if((gsmTime.charAt(k) != '+'))
    {
      time_buf[l]=gsmTime.charAt(k)-48;
      l++;
    }    
    k++;    
  }

  if((time_buf[0]!=208))          // To solve time glitch issue, getting 208=(256-48)
  { 
      while (j < 6)
      {    
          if((gsmTime.charAt(j) != '+'))
          {
              Ary[i]=gsmTime.charAt(j)-48;
              i++;
          }    
          j++;    
      }
      
      j=0;
      i=0;  
      while (j < 9)
      {    
        Ary_date[i]=gsmDate.charAt(j)-48;
        i++;
        j++;      
      } 
    
      hr1=Ary[0];
      hr2=Ary[1];  
      hr1=hr1<<4;   
      hr_GSM=hr1|hr2;
      
      if(hr_GSM>=16 && hr_GSM<=25)
      {
        hr_GSM=hr_GSM-6;  
      }
      
      if(hr_GSM>=32 && hr_GSM<=41)
      {
        hr_GSM=hr_GSM-12; 
      } 
  }   
}
//***********************************************************
void GSM_time_display()
{
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_osb35_tn);    
    u8g2.setCursor(0, 63);
    u8g2.print(Ary[0]);
    u8g2.print(Ary[1]); 
    u8g2.print(":");
    u8g2.print(Ary[3]);
    u8g2.print(Ary[4]);  
    u8g2.setFont(u8g2_font_fub14_tf); 
    u8g2.setCursor(06, 15);
    u8g2.print(Ary_date[6]);
    u8g2.print(Ary_date[7]);
    u8g2.print("/");
    u8g2.print(Ary_date[3]);
    u8g2.print(Ary_date[4]);
    u8g2.print("/");
    u8g2.print('2');
    u8g2.print('0');
    u8g2.print(Ary_date[0]);
    u8g2.print(Ary_date[1]); 
    while ( u8g2.nextPage() );
    smart_delay(20);
}
//***********************************************************
void RF_send()
{    
    uint16_t rf_address;               //1010
    uint16_t rf_data_process;         //1111 - 4bit address, 1111 1111 1111 - 12bit data (4096)    
    uint16_t light_Value=0;
    
    rf_address=1;                                
    light_Value = (rf_address << 14)|power_send;    
    if (!driver.send((uint8_t*)&light_Value, sizeof(light_Value)));
    driver.waitPacketSent();
    
    delay(250);
    rf_address=0;                                
    light_Value = (rf_address << 14)|kwhr_temp;    
    if (!driver.send((uint8_t*)&light_Value, sizeof(light_Value)));
    driver.waitPacketSent();    
    delay(50);
}
//************************************************************
void ATM_EMS_measure()
{    
    volt_temp    = eic.GetLineVoltage();    
    volt_send    = volt_temp;                           
    power_temp   = eic.GetActivePower();
    power_send   = power_temp;
    current_temp = eic.GetLineCurrent(); 
    freq_temp    = eic.GetFrequency();
    pf_temp      = eic.GetPowerFactor();

    volt_temp=eic.GetLineVoltage();
    if(!(volt_temp>30)&&(volt_temp<290))
    {
      eic.InitEnergyIC();
      delay(500);
    }
    
    volt_temp=eic.GetLineVoltage();
    if(!(volt_temp>30)&&(volt_temp<290))
    {
      eic.InitEnergyIC();
      delay(500);
    }
    
    unsigned short s_status = eic.GetSysStatus();
    if(s_status == 0xFFFF)
    {   
      eic.InitEnergyIC();
      delay(100); 

      s_status = eic.GetSysStatus();

      u8g2.clearBuffer();         
      u8g2.setFont(u8g2_font_fub17_tf);   
      u8g2.setCursor(35, 36);
      u8g2.print("EMS");
      u8g2.setCursor(15, 63);
      u8g2.print(s_status);
      while ( u8g2.nextPage() );
      delay(2000);
       
      if(s_status == 0xFFFF)
      {
        ESP.restart();   
      }
    }    
       
    if(max_power<power_temp)
    {
        max_power=power_temp;
        hr_buf=hr_GSM;        //tweaked for GSM
        //mint_buf=mint;
        Ary_mint[0]=Ary[0];
        Ary_mint[1]=Ary[1];
    }     
}
//*********************  Debug Screen ***********************
void debug_screen()
{    
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fub14_tf);//u8g2.setFont(u8g2_font_profont22_mf);
    u8g2.setCursor(0, 15);    
    u8g2.print("Wifi:");    
    if(no_wifi==1) 
    u8g2.print("NO ");
    else u8g2.print("YES ");
    u8g2.print(FW_VERSION);
    
    u8g2.setFont(u8g2_font_fub11_tf);   
    if(no_wifi==0)
    { 
        u8g2.setCursor(0, 31);    
        u8g2.print("RSSI: ");
        rssi = WiFi.RSSI();
        u8g2.print(rssi);
    }
    u8g2.setCursor(0, 47);
    u8g2.print("RT: ");
    if(10>hr_start) u8g2.print("0"); 
    u8g2.print(hr_start);
    u8g2.print(":");
    if(10>mint_start) u8g2.print("0");  
    u8g2.print(mint_start);    

    if(no_wifi==0)
    {
        u8g2.setFont(u8g2_font_fub11_tf);
        u8g2.setCursor(00, 63);
        u8g2.print(WiFi.localIP());
    }   
    while ( u8g2.nextPage() );        
    smart_delay(20);          
}
//***********************************************************
void smart_delay(int delay_ms)
{
  int8_t N=0,M=0;   
  for(char z=0;z<=delay_ms;z++)
  {   
    //wifi_status_check();
    switch_input();
    rf_door_sensor_check();   
    //delay(60);
    wifi_status_check();
    //httpServer.handleClient();
    //dnsServer.processNextRequest();
    delay(70);           
    yield();
  } 
}
//*****************************************************************
void MQTT_Wifi()
{  
  if (WiFi.status()==WL_CONNECTED) 
  {      
    if(WiFi.localIP().isSet())
    {
      //bool ret = Ping.ping("www.google.com", 2);
      //if(ret = 1)
      {        
         if(first_time_wifi==1)
         {
              Cayenne.begin(CayenneUser, CayennePass, CayenneID_Wifi);                              
              first_time_wifi=0;
              yield();
         }                            
         
         Cayenne.loop();         
         long freeHeap = ESP.getFreeHeap();          
         NRGY_send=(energy_temp+energy_temp_float);

         rf_door_sensor_check();
                         
         //Cayenne.virtualWrite(1, temperature_temp, "temperature", "c"); 
         //Cayenne.virtualWrite(12, , "analog_sensor", "%");
         //Cayenne.virtualWrite(11, freeHeap, "bytes", "byte");
         //Cayenne.virtualWrite(13, Ldist, "rel_hum", "p");
         Cayenne.virtualWrite(0, rssi, "rssi", "dbm");           
         Cayenne.virtualWrite(1, wlc_1, "cm", "cm");        
         Cayenne.virtualWrite(2, vbatt_1, "voltage", "v");
         Cayenne.virtualWrite(10, vin_1_temp, "voltage", "v");
         Cayenne.virtualWrite(11, temp_1, "temperature", "c");
         yield();
         
         if(volt_temp>80)
         {
             Cayenne.virtualWrite(3, power_temp, "pow", "w");   
             Cayenne.virtualWrite(4, NRGY_send, "energy", "kwh");
             Cayenne.virtualWrite(5, volt_temp, "voltage", "v");    
             Cayenne.virtualWrite(6, current_temp, "current", "a");   
             Cayenne.virtualWrite(7, pf_temp, "analog_sensor", "Ldist");
             Cayenne.virtualWrite(8, freq_temp, "freq", "hz");   
             Cayenne.virtualWrite(9, todays_energy_disp, "energy", "kwh");         
         }
         yield();                      
  
         u8g2.clearBuffer();  
         u8g2.setFont(u8g2_font_fub14_tf);
         u8g2.setCursor(24, 15); 
         u8g2.print(" Cloud ");
         u8g2.setFont(u8g2_font_fub17_tf);
         u8g2.setCursor(05, 56);
         u8g2.print("Wifi Sent !");
         while ( u8g2.nextPage() );
         delay(500);
      }
    }     
  }         
}
//***********************************************************
void GSM_cloud()
{ 
  String gsmTime = modem.getGSMDateTime(DATE_TIME);
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(24, 15); 
  u8g2.print(" Cloud ");
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(00, 58);
  u8g2.print("GPRS Cont..");      
  while ( u8g2.nextPage() );
  delay(250);  
 
  if (!modem.isGprsConnected()) 
  {
    yield();
    modem.gprsConnect("airtelgprs", "", "");
    yield();
    return;
    modem_restart++;
  }    
  else delay(1000);
  rf_door_sensor_check(); 
  yield(); 

  //u8g2.setCursor(05, 58);
  //u8g2.print("Uploading..");
  
  
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(mqttCallback); 

  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.setCursor(24, 15); 
  u8g2.print(" Cloud ");
  u8g2.setFont(u8g2_font_fub14_tf);  
  u8g2.setCursor(05, 58);
  u8g2.print("Uploading...");
  while ( u8g2.nextPage() );
  
  mqttConnect(); 
  rf_door_sensor_check(); 
  
 if (!mqtt.connected()) 
  {   
     yield();
     mqttConnect(); 
  }
  mqtt.loop();
  yield();  
  
  if((modem_restart>3))
  {      
    modem.restart();
    GSM_init();
    modem_restart=0;
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(24, 15); 
    u8g2.print(" MODEM ");
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(15, 58);
    u8g2.print("RESET!!");  
    while ( u8g2.nextPage() );
    delay(2000); 
    modem_reset_try++;  
  }

}
//**************************************************************/
boolean mqttConnect() 
{  
  int wifi_state_display;
  String local = modem.getLocalIP();
  
  boolean status = mqtt.connect(CayenneID, CayenneUser, CayennePass);
  yield();
  
  if (status==true) 
  {     
      
      SendSensorValue("38", "rssi,dbm=" + String(GSM_RSSI)); 
      SendSensorValue("31", "analog_sensor,null=" + String(water_level));       
      SendSensorValue("32", "rel_hum,p=" + String(humidity_temp));       
      SendSensorValue("42", "temp,c=" + String(minutes_buf));
      //delay(10);
      
      if(volt_temp>80)
      {
        SendSensorValue("33", "pow,w=" + String(power_temp));         
        SendSensorValue("35", "voltage,v=" + String(volt_temp));           
        SendSensorValue("36", "freq,hz=" + String(freq_temp));                
        SendSensorValue("39", "current,a=" + String(current_temp));
        if(ERGY_send==1)
        {
          SendSensorValue("34", "energy,kwh=" + String(NRGY_send));
          SendSensorValue("37", "energy,kwh=" + String(todays_energy_disp));
        }
        SendSensorValue("40", "analog_sensor,null=" + String(pf_temp));//water_level
        delay(10);
      }           
      if(no_wifi==0) wifi_state_display=1;   
      else wifi_state_display=0;       
      
      SendSensorValue("41", "digital_sensor,d=" + String(wifi_state_display));
      
      if( (Tsense==374)&&(vbatt_send==210)||(Tsense==0)&&(vbatt_send==0))
      {
        ;
      }
      else
      {
        SendSensorValue("43", "temp,c=" + String(Tsense));            
        SendSensorValue("44", "analog_sensor,null=" + String(vbatt_send)); 
      }
      SendSensorValue("45", "analog_sensor,null=" + String(FW_VERSION));        
      delay(10);        

      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.setCursor(24, 15); 
      u8g2.print(" Cloud ");
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.setCursor(05, 58);
      u8g2.print("GSM  Sent !");  
      while ( u8g2.nextPage() );
      yield();
      //SerialAT.println("AT+CIPCLOSE");
      delay(500);
      //smart_delay(3);      
      return mqtt.connected();
  }
  else
  {
      u8g2.clearBuffer();  
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.setCursor(24, 15); 
      u8g2.print(" Cloud ");
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.setCursor(15, 58);
      u8g2.print("ERROR !!");  
      while ( u8g2.nextPage() );
      delay(2000);
      modem_restart++;
  }
    
}
//*******************************************************************
void SendSensorValue(String channel, String data) 
{   
    String outtopic = "v1/" + String(CayenneUser) + "/things/" + String(CayenneID) + "/data/" + channel; 
    mqtt.publish(outtopic.c_str(),  data.c_str());  
}
//*******************************************************************
void mqttCallback(char* topic, byte* payload, unsigned int len) 
{
  //if (String(topic) == topicLed) 
  {
    //ledStatus = !ledStatus;
    //digitalWrite(LED_PIN, ledStatus);
    //mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
  }
}
//********************************************************************
void GSM_init()
{  
  SerialAT.begin(9600); 
  modem.init();
  delay(500);
  yield();
  
  String modemInfo = modem.getModemInfo();
  if (!modem.waitForNetwork()) //15sec
  {
    delay(100);
    yield();
    return;
  }
  if (modem.isNetworkConnected());   
  yield();
  
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(mqttCallback);
  
  //GSM_RSSI_Bars();       
}
//***********************************************************
void GSM_RSSI_Bars()
{
  char n = modem.getSignalQuality();     
  char bars;
  char barsw; 
  
  if (n == 0) GSM_RSSI = -115;
  if (n == 1) GSM_RSSI = -111;
  if (n == 31) GSM_RSSI = -52;
  //if (n >= 32) GSM_RSSI = -115;
  if ((n >= 2) && (n <= 30)) 
  {
    GSM_RSSI = map(n, 2, 30, -110, -54);
  } 
  
       if (GSM_RSSI > -30) bars = 5;
  else if (GSM_RSSI < -45 & GSM_RSSI > -65) bars = 4;
  else if (GSM_RSSI < -65 & GSM_RSSI > -75) bars = 3;
  else if (GSM_RSSI < -75 & GSM_RSSI > -85) bars = 2;
  else if (GSM_RSSI < -85 & GSM_RSSI > -96) bars = 1;
  else bars = 0;
  if (GSM_RSSI==0) bars = 0;  
 
  bars= bars*2;
  
  u8g2.clearBuffer();    
  u8g2.setFont(u8g2_font_profont22_tf); 
  
  u8g2.setCursor(0, 5);
  for (char b=0; b <= bars; b++) 
  {       
    fillRect(20+(b*5), 60-(b*5), 3, b*5);                  
  }
  u8g2.setFont(u8g2_font_fub11_tf); 
  u8g2.setCursor(0, 15);
  u8g2.print("G: ");
  u8g2.setFont(u8g2_font_fub14_tf);       
  u8g2.print(GSM_RSSI);

  //if(no_wifi==0)    
  {
    if (rssi > -30) barsw = 5;  
    else if (rssi < -45 & rssi > -65) barsw = 4;
    else if (rssi < -65 & rssi > -75) barsw = 3;
    else if (rssi < -75 & rssi > -85) barsw = 2;
    else if (rssi < -85 & rssi > -96) barsw = 1;
    else bars = 0;
  
    u8g2.setCursor(65, 5);
    for (char b=0; b <= barsw; b++) 
    {       
      fillRect(85+(b*5), 60-(b*5), 3, b*5);                  
    }
    u8g2.setFont(u8g2_font_fub11_tf); 
    u8g2.setCursor(66, 15);
    u8g2.print("W: ");  
    u8g2.setFont(u8g2_font_fub14_tf);      
    u8g2.print(rssi);
  }
  /*else
  {
    u8g2.setCursor(65, 5);
    for (char b=0; b <= barsw; b++) 
    {       
      fillRect(85+(b*5), 60-(b*5), 3, b*5);                  
    }
    u8g2.setFont(u8g2_font_fub11_tf); 
    u8g2.setCursor(65, 15);
    u8g2.print("W ");  
    u8g2.setFont(u8g2_font_fub14_tf);      
    u8g2.print("No");
  }*/
  while ( u8g2.nextPage() );       
  smart_delay(15);

   if (n > 5) GSM_present=1;
  else GSM_present=0;
}
//***********************************************************
void Remote_data_process()
{  
  uint16_t data1;
  uint8_t datalen = sizeof(data1);
  
  if(remote_door_sensor_code == 2538345)      // decoded code
  {
    ;  
  }
  else if (driver.recv((uint8_t*)&data1, &datalen) && datalen == sizeof(data1))
  {
      //rf_data=data1;  
      //RF_data_recv=0;    
  }    
    /*  rf_address = rf_data>>14;               //1010
      
      if(rf_address==3)                            //Address: 10 Reseerved for Light Sensor
      {
        Tsense = rf_data>>8;                //light_Value = (rf_address << 14)|(Tsense << 8)|distance; 
        Tsense = Tsense & 0x003F;           //light_Value = (rf_address << 14)|(Tsense << 8)|distance; 
        Udist = rf_data & 0x00FF;                    
      }

      if(rf_address==2)                            //Address: 10 Reseerved for Light Sensor
      {
        vbatt_send = rf_data>>8;                //light_Value = (rf_address << 14)|(Tsense << 8)|distance; 
        vbatt_send = vbatt_send & 0x003F;           //light_Value = (rf_address << 14)|(Tsense << 8)|distance; 
        Ldist = rf_data & 0x00FF;                    
      }      
      u8g2.clearBuffer();   
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.setCursor(10, 15); 
      u8g2.print(rf_data);
      u8g2.setCursor(00, 35);       
      u8g2.print(Tsense);
      u8g2.print(" | ");
      u8g2.print(Udist);
      u8g2.setCursor(00, 55);
      u8g2.print(vbatt_send);
      u8g2.print(" | ");
      u8g2.print(Ldist);       
      while ( u8g2.nextPage() );      
      smart_delay(20); */ 
   
}
//************************************************************
/*int RF_to_data()
{   
    char j, k;    
    int RF_buf[8]={0,0,0,0,0,0,0,0}; 
    RF_value=0;
    j=0,k=0;     
               
    //it++;
    it++;    
    while(! ((RF_buffer_array[it]==',') || (RF_buffer_array[it]==13)) )       //V:226,P:166,E:155#  //L:95,T:290,V:92#
    {                                                  
      RF_buf[j]= (RF_buffer_array[it]-48);          
      it++;
      j++;         
    }    
    while(!(j==0))       
    {
      RF_value+=( (RF_buf[j-1]*pow(10, k)));
      j--;
      k++;     
    }
    //it++;                 
    return RF_value;                           
}*/

//************************************************************************************
void EEPROM_put(int address, long value_write)
{      
  inputBytes[0] = (value_write & 0xFF);
  inputBytes[1] = ((value_write >> 8) & 0xFF);
  inputBytes[2] = ((value_write >> 16) & 0xFF);
  inputBytes[3] = ((value_write >> 24) & 0xFF);
  
  eeprom->writeBytes(address, 4, inputBytes);  
  delay(250);   
}
//************************************************************************************
long EEPROM_get(long address)
{  
  value_read=0;
  long four = eeprom->readByte(address);
  long three = eeprom->readByte(address + 1);
  long two = eeprom->readByte(address + 2);
  long one = eeprom->readByte(address + 3);
  delay(10); 
  return value_read = ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
//**********************************************************************************

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h) 
{ 
  for (int16_t i=x; i<x+w; i++) 
  {
    drawFastVLine(i, y, h);
  }
}
//*********************************************************************************
void drawFastVLine(int16_t x, int16_t y, int16_t h) 
{  
  u8g2.drawLine(x, y, x, y+h-1);
}
//*********************************************************************************
void switch_input()
{    
  buttonState = digitalRead(buttonPin);    
  if (buttonState == LOW) 
  {     
      delay(2000); 
      yield();          
 
       buttonState = digitalRead(buttonPin);   
       if (buttonState == LOW) 
       {
            if (WiFi.status() == WL_CONNECTED) 
            {
                u8g2.clearBuffer();  
                u8g2.setFont(u8g2_font_fub14_tf);
                u8g2.setCursor(30, 15); 
                u8g2.print("Wi-Fi");
                
                u8g2.setFont(u8g2_font_fub11_tf);
                u8g2.setCursor(0, 33);           
                u8g2.print(ssid);           
                
                u8g2.setFont(u8g2_font_fub11_tf);
                u8g2.setCursor(0, 48);
                u8g2.print(WiFi.localIP());
    
                long rssi = WiFi.RSSI();    
                //u8g2.clearBuffer();  
                u8g2.setFont(u8g2_font_fub11_tf);
                u8g2.setCursor(0, 63);
                u8g2.print(rssi);        
                      
                while ( u8g2.nextPage() );
                delay(2000); 
                yield(); 
            } 

            u8g2.clearBuffer();  
            u8g2.setFont(u8g2_font_fub14_tf);
            u8g2.setCursor(10, 15);
            u8g2.print("  SW Ver"); 
            u8g2.setFont(u8g2_font_fub14_tf);
            u8g2.setCursor(00, 39);
            u8g2.print("CFV: ");
            u8g2.print(FW_VERSION);
            u8g2.setCursor(00, 63);
            u8g2.print("LFV: ");
            u8g2.print(newVersion);          
            while ( u8g2.nextPage() );
            delay(2000);
            yield();

            debug_screen();
            yield();
        }
        //checkForUpdates();
      }    
}
//********************************************************************************************************************
/*boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password) 
{     
  fonaSerial->begin(9600);
  delay(100);  
  fona.begin(*fonaSerial);
  delay(100);
  
  fona.println("AT+CMEE=2");   
  
  int x=0;
  while( (fona.getNetworkStatus() != 1) && x<20)
  {
   delay(500);
   yield(); 
   x++;
  }      
  delay(2500);  
  yield();
     
  fona.setGPRSNetworkSettings(apn, username, password);
  delay(1000);
  
  Serial.println(F("Disabling GPRS"));  
  fona.enableGPRS(false);  
  delay(2500);  
  yield();
  delay(2500); 
  yield();  
  Serial.println(F("Enabling GPRS"));
  delay(2500); 
  yield();

  if (!fona.enableGPRS(true)) 
  {
    Serial.println(F("GPRS Fail !"));
    yield();  
    return false;
  }  
  return true;
}
//***********************************************************************************************************************

/*void MQTT_connect() 
{
  int8_t ret; 
   
  if (mqtt.connected()) 
  {     
     yield();     
  }  
  uint8_t retries = 3;  
  while ((ret = mqtt.connect()) != 0) 
  {           
     mqtt.disconnect();
     yield();
     delay(1500);             // wait 5 seconds
     retries--;
     if (retries == 0) 
     {      
       return;      
     }
     yield();
  }
}*/
//************************************************************

void i2c_scanner()
{
  byte count = 0, curser=0;  
  
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_fub11_tf);
  u8g2.setCursor(30, 28);   
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {        
            
       u8g2.print("0x");
       u8g2.print(i, HEX); 
       delay(1); 
       count++;
       curser++;   
       u8g2.setCursor(30, (28+curser*12));
       yield();
    } 
  }
  //curser++;
  //u8g2.setCursor(38, 12);
  //u8g2.print(count, DEC);  
  while ( u8g2.nextPage() );
  delay(2000);
  yield();
} 
//*************************************************************
/*
void WLC()
{                
   char status;
   double T,P,p0,a;
   
   bmp280.startForcedConversion();                      // Start BMP280 forced conversion (if we're in SLEEP_MODE)
   if (bmp280.getMeasurements(temperature, pressure, altitude))    // Check if the measurement is complete
    {               
          pressure_bmp280=pressure+2.5;                     // high level calibration with mobile
          abs_pressure= pressure_bmp280-pressure_int;
          water_level = abs_pressure*1.174;               //27th June
          if(water_level<1) water_level=0; 
          u8g2.clearBuffer();                  
          u8g2.setFont(u8g2_font_8x13O_mr);
          u8g2.setCursor(00, 14);
          u8g2.print((pressure_bmp280-900),1);
          u8g2.setCursor(90, 14);
          u8g2.print((pressure_int-900),1);
          u8g2.setCursor(48, 14);
          u8g2.print((pressure_bmp280-pressure_int),1);                 
          u8g2.setFont(u8g2_font_fub35_tf);
          if(water_level<=9)
          { 
            u8g2.setCursor(38, 63);
          }
          else  
          {
            u8g2.setCursor(26, 63);                
          }
          u8g2.print(water_level,0); 
          u8g2.setFont(u8g2_font_fub14_tf);
          u8g2.setCursor(85, 63);
          u8g2.print(" cm");   //     
          //u8g2.setCursor(0, 63);
          //u8g2.print(all_display);                   
          while ( u8g2.nextPage() ); 
          delay(200);    
  }
  //-----------
  {
      status = pressure_bmp180.startTemperature();
      if (status != 0)
      {          
        delay(status);      
        status = pressure_bmp180.getTemperature(T);
        if (status != 0)
        {     
          status = pressure_bmp180.startPressure(3);
          if (status != 0)
          {             
            delay(status);      
            status = pressure_bmp180.getPressure(P,T);
            if (status != 0)
            {
              pressure_int=P-10;                    // high level calibration with mobile
              //smart_delay(1);
              //yield();
            }            
          }
        } 
      }     
 //------------
  }   
  smart_delay(2);      
}
*/
