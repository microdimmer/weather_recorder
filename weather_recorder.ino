#include <SPI.h>
#include <Adafruit_sensor.h>
#include <DHT.h>
#include <SdFat.h>
#include <iarduino_RTC.h>
#include <SimpleTimer.h> 
#include <U8glib.h>

#include "graphics.h"

DHT dht(2, DHT22); //Aosong AM2320, D2 pin
iarduino_RTC time(RTC_DS3231); //SDA A4 pin, SCL A5 pin
U8GLIB_PCD8544 u8g(4, 5, 6); //RST D6 pin, CE D4 pin, DC (data/commands) D5 pin,CLK D13 pin, DIN D11 pin PCD8544 48x84
SdFat SD; //CS D10 pin, SCK D13 pin, MISO D12 pin, MOSI D11 pin

SimpleTimer timer;
File dataFile;
float freeSpace; 
float h,t = 0;
char h_str[4];
char t_str[4];
char dataString[6][32];
uint8_t index = 0;

bool cardInserted = 0;
bool dotsBlink = false;



void dateTime(uint16_t* fileDate, uint16_t* fileTime) { //callback for timestamps. access, creation, and modify, are set when a file is created. 
  *fileDate = FAT_DATE(static_cast<uint16_t>(2000+time.year), time.month, time.day); //return date using FAT_DATE macro to format fields
  *fileTime = FAT_TIME(time.Hours, time.minutes, time.seconds); //return time using FAT_TIME macro to format fields
}

void draw() {
  char out_str[5];
  time.gettime(); //refresh internal clock
  u8g.firstPage();  
  do {
    u8g.drawXBMP(0, 20, 16, 14, hum_bitmap);
    u8g.drawXBMP(0, 34, 8, 14, temp_bitmap);
    u8g.setFont(u8g_font_courB12r);
    u8g.drawStr(20, 34, h_str);
    u8g.drawStr(10, 48, "+");
    u8g.drawStr(20, 48, t_str);

    dotsBlink ? sprintf(out_str,"%d %d",time.Hours,time.minutes) : sprintf(out_str,"%d:%d",time.Hours,time.minutes); //draw time
    u8g.drawStr(0, 12, out_str);
    // u8g.setFont(u8g_font_courB08r);
    // sprintf(out_str,"%4.1f",freeSpace); //draw free space
    // u8g.drawStr(0, 48, "free");
    // u8g.drawStr(10, 48, out_str);

    // u8g.drawStr(10, 20, out_str);

  } while( u8g.nextPage() );
  dotsBlink ^=1; //dots blicking
}

void readData() {
  h = dht.readHumidity();
  if (!isnan(h)) 
    dtostrf(h, 4, 1, h_str);
  t = dht.readTemperature();
  if (!isnan(t))
    dtostrf(t, 4, 1, t_str);
  sprintf(dataString[index],"%s,%s,%s",h_str,t_str,gettime("d-m-Y,H:i:s"));//fill data string array like 45.80,7.50,21-11-2017,19:19:43
  if (++index >= 6) index = 0;
}

void writeData() {
  if (!SD.begin(SS))
    return;
  if (dataFile.open("data.txt", O_WRITE | O_CREAT)) {
    for (uint8_t i = 0; i <= 6; i++) {
      dataFile.seekEnd();
      dataFile.write(dataString[i]);
    }
  dataFile.close();
  freeSpace = 0.000512*SD.vol()->freeClusterCount()*SD.vol()->blocksPerCluster(); //MB (MB = 1,000,000 bytes)  
  }
}

void setup() {
  Serial.begin(9600);

  u8g.setContrast(110);

  time.begin();  
  time.period(10); //poll module once a 30 min

  dht.begin();

  // SD.begin(SS);
  dataFile.dateTimeCallback(dateTime); //set timestamp callback

  readData();

  timer.setInterval(1000L, draw);
  timer.setInterval(5000L, readData);
  // timer.setInterval(6000L, writeData);
}

void loop() {
  timer.run();
  // if (millis()%1000==0) {//draw once a second
  //   draw();
    
  // }
  // if (time.seconds%5==0) // 5 sec
    // readData();
  // delay(1);
  // if (millis()%55000==0) //1 hour
    // writeData();
}
