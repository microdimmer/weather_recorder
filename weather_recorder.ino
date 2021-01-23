#include <SPI.h>
#include <Adafruit_Sensor.h>  //https://github.com/adafruit/Adafruit_Sensor
#include <DHT.h> // https://github.com/adafruit/DHT-sensor-library
#include <SdFat.h>  // https://github.com/greiman/SdFat (version 1.1.4)
#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC /SDA A4 pin, SCL A5 pin
#include <SimpleTimer.h>  // https://playground.arduino.cc/Code/SimpleTimer/ or https://github.com/jfturcot/SimpleTimer
#include <U8glib.h> // https://github.com/olikraus/U8glib_Arduino

#include "graphics.h"

// #define DEBUG

DHT dht(2, DHT22); //Aosong AM2320, D2 pin
U8GLIB_PCD8544 u8g(4, 5, 6); //RST D6 pin, CE D4 pin, DC (data/commands) D5 pin,CLK D13 pin, DIN D11 pin PCD8544 48x84
SdFat SD; //CS D10 pin, SCK D13 pin, MISO D12 pin, MOSI D11 pin

SimpleTimer timer;
File dataFile;
float freeSpace; 
float h = NAN;
float t = NAN;
float min_t = NAN;
float max_t = NAN;
uint8_t max_day;
uint8_t max_month;
uint8_t min_day;
uint8_t min_month;

bool cardInserted = false;
bool dataWriteOK = false;
bool dotsBlink = false;

#ifdef DEBUG
#define PRINTLNF(s)   { Serial.println(F(s)); }
#define PRINTLN(s,v)  { Serial.print(F(s)); Serial.println(v); }
#define PRINT(s,v)  { Serial.print(F(s)); Serial.print(v); }
#else
#define PRINTLNF(s)
#define PRINTLN(s,v)
#define PRINT(s,v)
#endif

void dateTime(uint16_t* fileDate, uint16_t* fileTime) { //callback for timestamps. access, creation, and modify, are set when a file is created. 
 *fileDate = FAT_DATE(static_cast<uint16_t>(year()), month(), day()); //return date using FAT_DATE macro to format fields
 *fileTime = FAT_TIME(hour(), minute(), second()); //return time using FAT_TIME macro to format fields
}

float cardFreeSpace() { 
  return 0.000488 * SD.vol()->freeClusterCount() * SD.vol()->blocksPerCluster();  //MB (MB = 1,048,576 bytes)
  // return 0.000512*SD.vol()->freeClusterCount()*SD.vol()->blocksPerCluster(); //MB (MB = 1,000,000 bytes)
}

void draw() {
  char out_str[5];
  u8g.firstPage();  
  do {
    u8g.drawXBMP(0, 18, 16, 14, hum_bitmap);
    u8g.drawXBMP(0, 34, 8, 14, temp_bitmap);
    u8g.setFont(u8g_font_10x20r);

    sprintf(out_str,"%02d",hour()); //draw time
    u8g.drawStr(0, 13, out_str);
    if (dotsBlink) u8g.drawStr(18, 12, ":");
    sprintf(out_str,"%02d",minute());
    u8g.drawStr(27, 13, out_str);

    if (!isnan(h)) { //draw humidity
      sprintf(out_str,"%02d",static_cast<int>(h)); 
      u8g.drawStr(17, 31, out_str); //print decimal
      u8g.drawStr(33, 31, ".");
      dtostrf(h, 4, 1, out_str);
      u8g.drawStr(40,31, out_str+3); //print precision
    }
    if (!isnan(t)) { //draw temp
      (t > 0) ? u8g.drawStr(8, 48, "+") : u8g.drawStr(8, 48, "-");
      float t_abs = abs(t); 
      uint8_t x = 0;
      if (t_abs < 10) 
        x = 10;
      sprintf(out_str,"%d",static_cast<int>(t_abs));
      u8g.drawStr(17, 48, out_str); //print decimal
      u8g.drawStr(33-x, 48, ".");
      dtostrf(t_abs, 4, 1, out_str);
      u8g.drawStr(40-x, 48, out_str+3); //print precision
    }
    u8g.setFont(u8g_font_6x12r);
    if (max_t > 0) //print max temperature
      u8g.drawStr(53, 24, "+"); 
    else if (max_t < 0) 
      u8g.drawStr(53, 24, "-"); 
    dtostrf(abs(max_t), 4, 1, out_str);
    u8g.drawStr(60, 24, out_str); 
    if (min_t > 0) //print min temperature
      u8g.drawStr(53, 41, "+");
    else if (min_t < 0)
      u8g.drawStr(53, 41, "-"); 
    dtostrf(abs(min_t), 4, 1, out_str);
    u8g.drawStr(60, 41, out_str); 

    if (cardInserted) { //draw free space
      if (freeSpace>=1024) 
        dtostrf(freeSpace/1024, 3, 1, out_str);
      else if (freeSpace>=100) 
        sprintf(out_str,"%d",static_cast<int>(freeSpace));
      else 
         dtostrf(freeSpace, 4, 1, out_str); 
      u8g.drawStr(60, 7, out_str);
      u8g.setFont(u8g_font_4x6r);
      if (freeSpace>=1024)
        u8g.drawStr(57, 14, "free GB");
      else
        u8g.drawStr(57, 14, "free MB");
      if (dataWriteOK) {
         u8g.drawXBMP(51, 0, 7, 7, save_bitmap);  
      } 
    }

    u8g.setFont(u8g_font_4x6r); //draw min max date
    u8g.drawStr(53, 31, "max");
    sprintf(out_str,"%02d",max_day);
    u8g.drawStr(67, 31, out_str);
    u8g.drawStr(74, 31, ".");
    sprintf(out_str,"%02d",max_month);
    u8g.drawStr(77, 31, out_str);
    u8g.drawStr(53, 48, "min");
    sprintf(out_str,"%02d",min_day);
    u8g.drawStr(67, 48, out_str);
    u8g.drawStr(74, 48, ".");
    sprintf(out_str,"%02d",min_month);
    u8g.drawStr(77, 48, out_str);

  } while( u8g.nextPage() );
  dotsBlink ^=1; //dots blicking
}

void readData() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (!isnan(t)) {
    if (isnan(max_t) || (t > max_t)) {
      max_t = t;
      max_day = day();
      max_month = month(); 
    }
    if (isnan(min_t) || (t < min_t)) {
      min_t = t;
      min_day = day();
      min_month = month();
    }
  }
}

void writeBuff() {
  cardInserted = SD.begin(SS);
  if (!cardInserted) 
    return;

  char dataString[32];
  if (!isnan(h)) {
    dtostrf(h, 4, 1, dataString);
    dataString[4] = ',';
  } 
  if (!isnan(t)) {
    dtostrf(t, 4, 1, dataString+5);
    dataString[9] = ','; 
  }
  sprintf(dataString+10, "%02d-%02d-%d,%02d:%02d:%02d\r\n", day(), month(), year(), hour(), minute(), second()); //fill data string array like 45.80,7.50,21-11-2017,19:19:43
  PRINT("dataString:",dataString);
  if (!dataFile.isOpen())
    dataFile.open("data.txt", O_WRITE | O_CREAT);
  // else 
    dataFile.seekEnd();
  dataFile.write(dataString);
  PRINTLNF("buff write"); 
}

void writeCard() {
  cardInserted = SD.begin(SS);
  if (!cardInserted) 
    return;
  dataFile.sync() ? dataWriteOK = 1 : dataWriteOK = 0;
  dataFile.close();
  freeSpace = cardFreeSpace();
  PRINTLNF("card write"); 
}

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  setSyncProvider(RTC.get); //the function to get the time from the RTC
  setSyncInterval(1800); //once a 30 min sync

  u8g.setContrast(114);

  dht.begin();
  
  cardInserted = SD.begin(SS);
  if (cardInserted) 
    freeSpace = cardFreeSpace();
    
  PRINTLN("freeSpace = ",freeSpace);
  dataFile.dateTimeCallback(dateTime); //set timestamp callback for files
  
  readData();

  timer.setInterval(1000L, draw);
  timer.setInterval(5000L, readData); //5 sec
  timer.setInterval(600000L, writeBuff); //10 min
  timer.setInterval(3600000L, writeCard); //1 hour write data buff to card 
}

void loop() {
  timer.run();
}
