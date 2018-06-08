#include <SPI.h>
#include <Adafruit_sensor.h>
#include <DHT.h>
#include <SdFat.h>
#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC /SDA A4 pin, SCL A5 pin
#include <SimpleTimer.h> 
#include <U8glib.h>

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
bool cardInserted = false;
bool dotsBlink = false;

// const float typVbg = 0.95;

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
    if (!isnan(h)) {
      sprintf(out_str,"%02d",static_cast<int>(h)); 
      u8g.drawStr(18, 31, out_str); //print decimal
      u8g.drawStr(34, 31, ".");
      dtostrf(h, 4, 1, out_str);
      u8g.drawStr(42,31, out_str+3); //print precision
    }
    if (!isnan(t)) {
      (t > 0) ? u8g.drawStr(8, 48, "+") : u8g.drawStr(8, 48, "-");
      float t_abs = abs(t); 
      uint8_t x = 0;
      if (t_abs < 10) 
        x = 10;
      sprintf(out_str,"%d",static_cast<int>(t_abs));
      u8g.drawStr(18, 48, out_str); //print decimal
      u8g.drawStr(34-x, 48, ".");
      dtostrf(t_abs, 4, 1, out_str);
      u8g.drawStr(42-x, 48, out_str+3); //print precision
    }
    dotsBlink ? sprintf(out_str,"%02d %02d",hour(),minute()) : sprintf(out_str,"%02d:%02d",hour(),minute()); //draw time
    u8g.drawStr(0, 13, out_str);

    u8g.setFont(u8g_font_6x13Br); 
    if (max_t > 0) //print max temperature
      u8g.drawStr(54, 26, "+"); 
    else if (max_t < 0) 
      u8g.drawStr(54, 26, "-"); 
    dtostrf(abs(max_t), 4, 1, out_str);
    u8g.drawStr(60, 26, out_str); 
    if (min_t > 0) //print min temperature
      u8g.drawStr(54, 42, "+");
    else if (min_t < 0)
      u8g.drawStr(54, 42, "-"); 
    dtostrf(abs(min_t), 4, 1, out_str);
    u8g.drawStr(60, 42, out_str); 

    u8g.setFont(u8g_font_4x6r);
    u8g.drawStr(57, 32, "max");
    u8g.drawStr(70, 32, "t");
    u8g.drawStr(57, 48, "min");
    u8g.drawStr(70, 48, "t");


    if (cardInserted) { //draw free space
      u8g.setFont(u8g_font_6x13Br);
      if (freeSpace>=1024) 
        dtostrf(freeSpace/1024, 3, 1, out_str);
      else if (freeSpace>=100) 
        sprintf(out_str,"%d",static_cast<int>(freeSpace));
      else 
         dtostrf(freeSpace, 4, 1, out_str); 
      u8g.drawStr(59, 9, out_str);
      u8g.setFont(u8g_font_4x6r);
      if (freeSpace>=1024)
        u8g.drawStr(57, 15, "free GB");
      else
        u8g.drawStr(57, 15, "free MB");
    }
  } while( u8g.nextPage() );
  dotsBlink ^=1; //dots blicking
}

void readData() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  // t = -25.1;
  if (!isnan(t)) {
    if (isnan(max_t)) 
      max_t = t; 
    if (isnan(min_t)) 
      min_t = t;
    max_t =  max(t,max_t);
    min_t =  min(t,min_t);
  }
  // PRINTLN("Vcc= ",readVcc());
}

void writeBuff() {
  cardInserted = SD.begin(SS);
  if (!cardInserted) 
    return;

  char dataString[31];
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
  dataFile.seekEnd();
  dataFile.write(dataString);
  PRINTLNF("buff write"); 
}

void writeCard() {
  cardInserted = SD.begin(SS);
  if (!cardInserted) 
    return;

  dataFile.close();
  freeSpace = cardFreeSpace();
  PRINTLNF("card write"); 
}

// float readVcc() {
//   byte i;
//   float result = 0.0;
//   float tmp = 0.0;
//   for (i = 0; i < 5; i++) {
//     ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);// set the reference to Vcc and the measurement to the internal 1.1V reference
//     delay(3); // Wait for Vref to settle
//     ADCSRA |= _BV(ADSC); // Start conversion
//     while (bit_is_set(ADCSRA,ADSC)); // measuring
//     uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
//     uint8_t high = ADCH; // unlocks both
//     tmp = (high<<8) | low;
//     tmp = (typVbg * 1023.0) / tmp;
//     result = result + tmp;
//     delay(5);
//   }
//   result = result / 5;
//   return result;
// }

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  setSyncProvider(RTC.get); //the function to get the time from the RTC
  setSyncInterval(1800); //once a 30 min sync

  u8g.setContrast(115); //TODO add auto contrast

  dht.begin();
  
  cardInserted = SD.begin(SS);
  if (cardInserted) 
    freeSpace = cardFreeSpace();
    
  PRINTLN("freeSpace = ",freeSpace);
  dataFile.dateTimeCallback(dateTime); //set timestamp callback for files
  
  readData();

  timer.setInterval(1000L, draw);
  timer.setInterval(5000L, readData); //5 sec
  // timer.setInterval(10000L, writeBuff); 
  timer.setInterval(600000L, writeBuff); //10 min
  timer.setInterval(3600000L, writeCard); //1 hour write data buff to card 
}

void loop() {
  timer.run();
}
