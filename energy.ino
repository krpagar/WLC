void bill_display()
{            
    int bill_per_month=0;
    int bill_per_month_calc1=0;
    int bill_cal_excess=0;
    
    bill_per_month=(energy_temp+energy_temp_float)/1000;
    if(bill_per_month>100)
    {
      bill_cal_excess= (bill_per_month-100);
      bill_cal_excess= bill_cal_excess * 7.43;
    }
    yield();
        
    bill_per_month_calc1 = (((bill_per_month*3.46)+bill_cal_excess)+ 110 + (1.45*bill_per_month));
    bill_per_month = (bill_per_month_calc1 * 0.16)+ bill_per_month_calc1; 
    
    yield();   
    
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf );
    
    if(bill_per_month<999)
    u8g2.setCursor(22, 63);
    else u8g2.setCursor(02, 63);
    u8g2.print(bill_per_month);
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(47, 15);
    u8g2.print("Rs.");          
    while ( u8g2.nextPage() );    
    smart_delay(15); 
}
//********************************************************
void time_display()
{
    if(no_wifi==0)
    {
      timeClient.update();
      delay(10);
      //yield();
    }    
    hr=timeClient.getHours();
    mint=timeClient.getMinutes(); 
  
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fur35_tf );    
    u8g2.setCursor(0, 63);
    if(10>hr) u8g2.print("0"); 
    u8g2.print(hr); 
    u8g2.print(":");
    if(10>mint) u8g2.print("0");  
    u8g2.print(mint);  
    u8g2.setFont(u8g2_font_fub14_tf); 
    u8g2.setCursor(25, 15);
    u8g2.print("Hr  :");
    u8g2.setCursor(72, 15);
    u8g2.print(" Min");  
    while ( u8g2.nextPage() );
    smart_delay(15);      
}
//*************************************************************
void temp_hum_display()
{  
    delay(dht.getMinimumSamplingPeriod());
    delay(10); 
    yield();
     
    humidity_temp = dht.getHumidity(); 
    temperature_temp = dht.getTemperature();

    yield();
    
    u8g2.clearBuffer();   
    u8g2.setFont(u8g2_font_fur35_tf ); 
    u8g2.setCursor(1, 63);
    u8g2.print(temperature_temp,1);
    u8g2.setCursor(71, 63);
    u8g2.println(humidity_temp, DEC); 
    u8g2.setFont(u8g2_font_fub14_tf);  
    u8g2.setCursor(5, 15);
    u8g2.print("  °C");
    u8g2.setCursor(87, 15);
    u8g2.print("%");  
    while ( u8g2.nextPage() );
    
    smart_delay(15);  
} 
//**************************************************************
void energy_display()
{
    char temp[6];
    char buffer1[6];
    char j,k;
    current_enrgy=0;
    data_buf=0;
    
    data_buf = (energy_temp+energy_temp_float);  
    kwhr_temp = data_buf;  

    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf );
    u8g2.setCursor(01, 63);
    
    if((energy_temp+energy_temp_float)>99999)                  
    {                  
        data_buf = (data_buf / 100); 
        kwhr_temp = data_buf;               
                    
        temp[3]=(data_buf%10);        
        data_buf=data_buf/10;
        temp[2]=(data_buf%10);
        data_buf=data_buf/10;
        temp[1]=(data_buf%10);
        temp[0]=(data_buf/10);
        sprintf(buffer1,"%d%d%d.%d", temp[0],temp[1],temp[2],temp[3]);
        mul_factor=100;    
    }
    else if((energy_temp+energy_temp_float)>9999)
    {                   
        data_buf = (data_buf / 10);
        kwhr_temp = data_buf;                   
                    
        temp[3]=(data_buf%10);        
        data_buf=data_buf/10;
        temp[2]=(data_buf%10);
        data_buf=data_buf/10;
        temp[1]=(data_buf%10);
        temp[0]=(data_buf/10);
        sprintf(buffer1,"%d%d.%d%d", temp[0],temp[1],temp[2],temp[3]);
        mul_factor=10; 
    }
    else if((energy_temp+energy_temp_float)>999)
    {                     
        temp[3]=(data_buf%10);        
        data_buf=data_buf/10;
        temp[2]=(data_buf%10);
        data_buf=data_buf/10;
        temp[1]=(data_buf%10);
        temp[0]=(data_buf/10);
        sprintf(buffer1,"%d.%d%d%d", temp[0],temp[1],temp[2],temp[3]); 
        mul_factor=1;  
        //kwhr_temp = data_buf;
    }
    else if((energy_temp+energy_temp_float)>99)
    {       
        temp[3]=(data_buf%10);         
        data_buf=data_buf/10;
        temp[2]=(data_buf%10);        
        temp[1]=data_buf/10;
        temp[0]=0;       
        sprintf(buffer1,"%d.%d%d%d", temp[0],temp[1],temp[2],temp[3]);
        mul_factor=1; 
    }
    else if((energy_temp+energy_temp_float)>9)
    {                     
        temp[3]=(data_buf%10);         
        temp[2]=data_buf/10;
        temp[1]=0;        
        temp[0]=0;
        sprintf(buffer1,"%d.%d%d%d", temp[0],temp[1],temp[2],temp[3]);
        mul_factor=1;        
    }
    else if((energy_temp+energy_temp_float)>=0)
    {                     
        temp[3]=(data_buf%10);         
        temp[2]=0;
        temp[1]=0;        
        temp[0]=0;
        sprintf(buffer1,"%d.%d%d%d", temp[0],temp[1],temp[2],temp[3]);
        mul_factor=1;        
    }

    GSM_time();

    u8g2.print(buffer1);
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(42, 15);       
    u8g2.print("kWHr");   
    while ( u8g2.nextPage() );        
    smart_delay(20);
    //smart_delay(20);

    ERGY_write++;
    if(ERGY_write>7)                           // after 3mins ??
    {    
        
        ERGY_write=0;
        energy_temp_float+= float((eic.GetImportEnergy()/32));
        
        char address_energy=20;
        NRGY_send=0;        
        NRGY_send=(energy_temp+energy_temp_float);  
        
        if(ERGY_send==0)
        {
          last_enrgy_start=NRGY_send;         
        }
        ERGY_send=1;             
        
        EEPROM_put(address_energy, NRGY_send );    

        delay(200);
        EEPROM.commit();
        delay(200);

        EEPROM_wrote++;
        energy_temp_check=EEPROM_get(address_energy);
        delay(100);

        Serial.print("\nNRGY: ");
        Serial.println(energy_temp_float);
    }   
    
    current_enrgy = (energy_temp+energy_temp_float);    // convert to WHr   

        hr=timeClient.getHours();
    mint=timeClient.getMinutes();
    
    if( (hr==00) && (mint<30))   //10:24
    //if( (hr_GSM==00) && (Ary[3]<1) && (Ary_date[0]==2) && (Ary_date[1]==2))    //10:24
    {        
        delay(1000);
        yield();
        GSM_time();
            
       // if( (hr_GSM==00) && (Ary[3]<1) && (Ary_date[0]==2) && (Ary_date[1]==2))   //10:24
        { 
              if(only_once_here==0)
              {
                  only_once_here=1;        
                  int address_energy=30;   
          
                  second_day_ergy = first_day_ergy;
                  first_day_ergy = todays_energy_disp;         
                  
                  EEPROM_put(40, second_day_ergy );
                  delay(300);
                  EEPROM.commit();
                  delay(300);
          
                  EEPROM_put(10, first_day_ergy );
                  delay(300);
                  EEPROM.commit();
                  delay(300); 
          
                  yield();
                  NRGY_send=0;
                  NRGY_send=energy_temp+energy_temp_float;     
                  EEPROM_put(30, NRGY_send );
                  delay(300);
                  EEPROM.commit();
                  delay(300);
          
                  last_enrgy=EEPROM_get(address_energy);
                  delay(200);                  
                  yield();        
            
                  u8g2.clearBuffer();   
                  u8g2.setFont(u8g2_font_fub20_tf); 
                  u8g2.setCursor(1, 22);
                  u8g2.print("EEPROM"); 
                  u8g2.setFont(u8g2_font_fub20_tf);
                  u8g2.setCursor(10, 63);
                  u8g2.print("Wrote!");            
                  u8g2.print(store_enrgy[1]);
                  u8g2.print(store_enrgy[2]);
                  u8g2.print(store_enrgy[3]);
                  u8g2.print(store_enrgy[4]);      
                  while ( u8g2.nextPage() );
                  //delay(1500);
                  yield();
      
                  //GSM_init();
                  //yield();
              }
        }
    }
    else only_once_here=0;   
     
}
//**************************************************************
void today_energy_display()
{    
    diff_enrgy=0;
    char temp1[6];
    char buffer2[6]={0,0,0,0,0,0};
    char j,k;
    int energy_temp_diff;
    int mul_factor_today=0;     
    
    diff_enrgy = ((current_enrgy)-(last_enrgy));     // Whr
    todays_energy_disp=diff_enrgy;
    
    if(diff_enrgy<0)
    {
      diff_enrgy = ((current_enrgy*10)-last_enrgy);
    }
    
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fur35_tf );   

    if(diff_enrgy==0)
    {        
        u8g2.setCursor(46, 63);
        u8g2.print("0");        
    }
 /*     
    if(diff_enrgy>9999)
    {                        
        temp1[3]=(diff_enrgy%10);        
        diff_enrgy=diff_enrgy/10;
        temp1[2]=(diff_enrgy%10);
        diff_enrgy=diff_enrgy/10;
        temp1[1]=(diff_enrgy%10);
        temp1[0]=(diff_enrgy/10);
        sprintf(buffer2,"%d.%d%d%d", temp1[0],temp1[1],temp1[2],temp1[3]);
        u8g2.setCursor(05, 63);
        u8g2.print(todays_energy_disp);  
    }
*/    
    if(diff_enrgy>999)
    {                        
        temp1[3]=(diff_enrgy%10);        
        diff_enrgy=diff_enrgy/10;
        temp1[2]=(diff_enrgy%10);
        diff_enrgy=diff_enrgy/10;
        temp1[1]=(diff_enrgy%10);
        temp1[0]=(diff_enrgy/10);
        sprintf(buffer2,"%d.%d%d%d", temp1[0],temp1[1],temp1[2],temp1[3]);
        u8g2.setCursor(10, 63);
        u8g2.print(todays_energy_disp);  
    }
    else if(diff_enrgy>99)
    {            
        temp1[3]=(diff_enrgy%10);         
        data_buf=data_buf/10;
        temp1[2]=(diff_enrgy%10);      
        temp1[1]=diff_enrgy/10;
        temp1[0]=0;              
        sprintf(buffer2,"%d.%d%d%d", temp1[0],temp1[1],temp1[2],temp1[3]);
        u8g2.setCursor(20, 63);
        u8g2.print(todays_energy_disp);  
    }
    else if(diff_enrgy>9)
    {             
        temp1[3]=(diff_enrgy%10);         
        temp1[2]=diff_enrgy/10;
        temp1[1]=0;        
        temp1[0]=0;              
        sprintf(buffer2,"%d.%d%d%d", temp1[0],temp1[1],temp1[2],temp1[3]); 
        u8g2.setCursor(34, 63);
        u8g2.print(todays_energy_disp); 
    }
    else if(diff_enrgy>0)
    {             
        temp1[3]=(diff_enrgy%10);         
        temp1[2]=0;
        temp1[1]=0;        
        temp1[0]=0;              
        sprintf(buffer2,"%d.%d%d%d", temp1[0],temp1[1],temp1[2],temp1[3]);
        u8g2.setCursor(46, 63);
        u8g2.print(todays_energy_disp);  
    }    
    
    u8g2.setFont(u8g2_font_fub14_tf);           
    u8g2.setCursor(0, 15);       
    u8g2.print("TodaY's WHr");    
    while ( u8g2.nextPage() );   
    //smart_delay(25);
    smart_delay(30);     
}
//*************************************************************
void volt_display()
{            
    volt_temp    = eic.GetLineVoltage();
    if(volt_temp>30)
    volt_temp    =  volt_temp-7; 
    
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf );
    if(volt_temp<=0) u8g2.setCursor(45, 63);
    else u8g2.setCursor(22, 63);
    u8g2.print(volt_temp); 
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(42, 15);
    u8g2.print("Volt");          
    while ( u8g2.nextPage() );    
    smart_delay(30); 
}
//*************************************************************
void power_display()
{   
    power_temp   = eic.GetActivePower();   
    
    if(power_temp<0)
    {
      power_temp=power_temp*-1;
    }
    if((power_temp>=0) && (power_temp<2))
    {
      power_temp=0;
    }
    
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf ); 
           
    if(power_temp>999) u8g2.setCursor(04, 63); 
    else if(power_temp>99) u8g2.setCursor(22, 63);
    else if(power_temp>9) u8g2.setCursor(37, 63);
    else if(power_temp>=0) u8g2.setCursor(50, 63);

    u8g2.print(power_temp);      
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(42, 15);
    u8g2.print("Watt");          
    while ( u8g2.nextPage() );    
    //smart_delay(25);
    smart_delay(30); 
}
//*************************************************************
void max_power_display()
{   
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf );
            
    if(max_power>999) u8g2.setCursor(07, 63);
    else if(max_power>99) u8g2.setCursor(22, 63);
    else if(max_power>9) u8g2.setCursor(37, 63);
    else if(max_power>=0) u8g2.setCursor(50, 63);

    u8g2.print(max_power);      
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(0, 15);
    u8g2.print("Mx-PW ");
    //if(10>hr_buf) u8g2.print("0");    
    u8g2.print(hr_buf); 
    u8g2.print(":");
    u8g2.print(Ary_mint[0]);
    u8g2.print(Ary_mint[1]);  
    //u8g2.print(mint_buf);                  
    while ( u8g2.nextPage() );    
    //smart_delay(25);
    smart_delay(30); 
}
//*************************************************************
void current_display()
{
    int current_temp_disp;

    current_temp = eic.GetLineCurrent(); 
    
    if(current_temp<0)
    {
      current_temp=current_temp*-1;
    }
            
    current_temp = current_temp - 0.05;     //54.9mA is the OFFSET in READING not in POWER!  

    if(current_temp<0)
    {
      current_temp=0;
    }
    
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf );  
    
    if(current_temp>9.999) u8g2.setCursor(01, 63); 
    else u8g2.setCursor(16, 63); 
    
    u8g2.print(current_temp,2);      
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(55, 15);
    u8g2.print("A");          
    while ( u8g2.nextPage() );       
    
    smart_delay(20);
     
}
//*************************************************************
void freq_pf_display()
{
    freq_temp    = eic.GetFrequency();
    pf_temp      = eic.GetPowerFactor(); 

    if(pf_temp<0)
    {
      pf_temp=0;
    }
    u8g2.clearBuffer();   
    u8g2.setFont(u8g2_font_fub25_tf); 
    u8g2.setCursor(0, 58);
    u8g2.print(pf_temp,1);
    u8g2.setCursor(60, 58);
    u8g2.println(freq_temp, 1); 
    u8g2.setFont(u8g2_font_fub14_tf);  
    u8g2.setCursor(20, 15);
    u8g2.print("PF");
    u8g2.setCursor(83, 15);
    u8g2.print(" Hz");  
    while ( u8g2.nextPage() );    
    smart_delay(20);  
}
//*************************************************************
/*void energy_history_display()     //second_day_ergy
{    
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fub17_tf);   
    u8g2.setCursor(0, 39);
    u8g2.print("Yd: ");
    u8g2.print(first_day_ergy);
    //u8g2.print("kWH");        
    u8g2.setCursor(0, 63);
    u8g2.print("Td: ");
    u8g2.print(second_day_ergy);
    //u8g2.print("kWH");
    //u8g2.print("WHr");
    u8g2.setFont(u8g2_font_fub14_tf);   
    u8g2.setCursor(12, 15);
    u8g2.print(" WH LOG ");     
    while ( u8g2.nextPage() );    
    smart_delay(15);  
}*/
//*************************************************************
void thermometer_display()
{    
  //if(RF_data_recv<=5)
  { 
      rf_data_store=rf_data;
      static char outstr[6]; 
      
      //Tsense = rf_data_store >> 8 ;
      Tsense = Tsense*2; 
      Temp_float = (Tsense/10);      
      
      //vbatt_send =  rf_data_store & 0x00FF;      
      vbatt_send =  (vbatt_send*3);
      vbatt_send_float = (vbatt_send/100);             
         
      u8g2.clearBuffer();   
      u8g2.setFont(u8g2_font_profont22_mf);
      u8g2.setCursor(25, 15); 
      u8g2.print("> RF <");

      u8g2.setFont(u8g2_font_fub17_tf);
      u8g2.setCursor(00, 38);
     //u8g2.print("T: ");       
      //u8g2.print(Tsense);
      //u8g2.print(" cel");
      u8g2.print("P:");
      u8g2.print(rf_data_store);
      //u8g2.print("V");
      
      //u8g2.setCursor(00,63);
      //u8g2.print("V: ");
      //u8g2.print(vbatt_send);
      //u8g2.print(" mV");
      //u8g2.print(vbatt_send_float,2);

      //u8g2.setCursor(00,63);
      //u8g2.print("RF: ");
      //u8g2.print(rf_data_store);
      //u8g2.print(vbatt_print);       
      while ( u8g2.nextPage() );      
      smart_delay(17);  
      RF_data_recv++; 
  } 
}
//------------------------------------------------------------wlc_1
void WLC()
{   
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_fur35_tf );            
    
    if(wlc_1>99) u8g2.setCursor(22, 63);
    else if(wlc_1>9) u8g2.setCursor(37, 63);
    else if(wlc_1>=0) u8g2.setCursor(50, 63);

    u8g2.print(wlc_1);      
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.setCursor(0, 15);
    u8g2.print(">>WLC<< ");
    //if(10>hr_buf) u8g2.print("0");    
   // u8g2.print(hr_buf); 
    //u8g2.print(":");
    //u8g2.print(Ary_mint[0]);
    //u8g2.print(Ary_mint[1]);  
    //u8g2.print(mint_buf);                  
    while ( u8g2.nextPage() );    
    //smart_delay(25);
    smart_delay(30); 
}
//**************************************************************
/*void cycle_energy_display()
{    
    int diff_enrgy_cycle=0;
      
    diff_enrgy_cycle = ((current_enrgy)-(last_enrgy_start));     // Whr   
    
    if(diff_enrgy_cycle<0)
    {
      diff_enrgy_cycle = ((current_enrgy*10)-last_enrgy_start);
    }    
    u8g2.clearBuffer();         
    u8g2.setFont(u8g2_font_fur35_tf );   

    if(diff_enrgy_cycle==0)
    {        
        u8g2.setCursor(46, 63);
        u8g2.print("0");        
    }      
    else if(diff_enrgy_cycle>999)
    {     

        u8g2.setCursor(05, 63);
        u8g2.print(diff_enrgy_cycle);  
    }
    else if(diff_enrgy_cycle>99)
    {            

        u8g2.setCursor(20, 63);
        u8g2.print(diff_enrgy_cycle);  
    }
    else if(diff_enrgy_cycle>9)
    {             
         
        u8g2.setCursor(34, 63);
        u8g2.print(diff_enrgy_cycle); 
    }
    else if(diff_enrgy_cycle>0)
    {            

        u8g2.setCursor(46, 63);
        u8g2.print(diff_enrgy_cycle);  
    }    
    
    u8g2.setFont(u8g2_font_fub14_tf);           
    u8g2.setCursor(00, 15);       
    u8g2.print("E-CHG ");
    //u8g2.setCursor(50, 15);
    if(hours<10) u8g2.print("0");       
    u8g2.print(hours);
    u8g2.print(":");
    if(minutes<10) u8g2.print("0");
    u8g2.print(minutes);
    //u8g2.print(":");
    //if(seconds<10) u8g2.print("0");
    //u8g2.print(seconds);     
    while ( u8g2.nextPage() );   
    smart_delay(15);     
}*/
//*************************************************************
