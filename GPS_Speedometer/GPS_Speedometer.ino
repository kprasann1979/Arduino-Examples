//To upload into Nano I used Atmega328(Old Bootloader) option - FYI 
//1.8inch SPI TFT 128x160 - 1.RST-D7;2.CS-D9;3.D/C-D8;4.Din-D11;5.CLK-D13;6.Vcc-5v;7.BL-3.3v;8.Gnd-G
//GPS Module: Tx(usually green cable)-ArduinoRx(4 if Softwareserial);Rx(usually yellow cable)-ArduinoTx(3 if Softwareserial)

#include <TFT_ILI9163.h>    // https://github.com/Bodmer/TFT_ILI9163
#include <SPI.h>
#include <SoftwareSerial.h> // http://playground.arduino.cc/Code/SimpleTimer
#include <TinyGPS++.h>      // https://github.com/mikalhart/TinyGPSPlus
#include <SimpleTimer.h>    // http://playground.arduino.cc/Code/SimpleTimer


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

int lock = 0;
//uint32_t satVal;
int satVal;
char *speedStr = malloc(3);
char *satStr = malloc(2);
double latVal;double lngVal;

TFT_ILI9163 tft = TFT_ILI9163();       // Invoke custom library
SoftwareSerial SoftSerial(RXPin, TXPin);

TinyGPSPlus gps;
SimpleTimer timer;

void setup(void) 
{
  SoftSerial.begin(GPSBaud);
  Serial.begin(GPSBaud);
  tft.init();
  printHeader();
  printSpeedHeader();
  printInfoBoxes();
  timer.setInterval(5000, printInfo);
}

void loop() 
{
  timer.run();
  lock = 0;
  smartDelay(100);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (SoftSerial.available())
      gps.encode(SoftSerial.read());
    sprintf(speedStr,"%3d",(int)gps.speed.kmph());
    satVal = gps.satellites.value();
    sprintf(satStr,"%2d",satVal);
    if (satVal > 0) lock = 1;
    latVal = gps.location.lat();
    lngVal = gps.location.lng();
    printSpeed();          
    printLockStatStrip(lock);
  } while (millis() - start < ms);
}

void printInfo()
{
  printSats();
  printLoc();
  printHeight();
}

void printSpeed()
{
  tft.setTextColor(ILI9163_GREEN,ILI9163_BLACK);
  tft.setTextSize(4);tft.setCursor(20,40);tft.print(speedStr);
}

void printSats()
{
  tft.setTextColor(ILI9163_BLACK,ILI9163_CYAN);
  tft.setTextSize(1);tft.setCursor(2,102);tft.print(satStr);
}

void printLoc()
{
  tft.setTextColor(ILI9163_BLACK,ILI9163_YELLOW);
  tft.setCursor(66,90);tft.print(latVal);
  tft.setCursor(66,102);tft.print(lngVal);
}

void printHeight()
{
  tft.setTextColor(ILI9163_BLACK,ILI9163_ORANGE);
  tft.setCursor(124,102);tft.print(gps.altitude.meters());
}

void printHeader() 
{
  tft.fillScreen(ILI9163_BLACK);
  tft.setRotation(1);    
  tft.fillRoundRect(0,0,160,20,2,ILI9163_LIGHTGREY);
  tft.setTextColor(ILI9163_BLACK,ILI9163_LIGHTGREY); 
  tft.setTextSize(1);tft.setCursor(5,6);
  tft.print(" === GPS Speedometer === ");    
}

void printSpeedHeader() 
{
  tft.setTextWrap(false);
  tft.setTextColor(ILI9163_BLUE);
  tft.setTextSize(2);
  tft.setCursor(120, 26);tft.println(" KM");
  tft.setCursor(120, 43);tft.println(" /");     
  tft.setCursor(120, 63);tft.println(" H");  
}

void printInfoBoxes() 
{
  tft.setTextSize(1);  
  tft.fillRoundRect(0,85,38,30,2,ILI9163_CYAN);
  tft.setTextColor(ILI9163_BLACK,ILI9163_CYAN);  
  tft.setCursor(2,90);tft.print("Sat #");
  tft.fillRoundRect(40,85,78,30,2,ILI9163_YELLOW);  
  tft.setTextColor(ILI9163_BLACK,ILI9163_YELLOW);    
  tft.setCursor(42,90);tft.print("Lat: ");  
  tft.setCursor(42,102);tft.print("Lng: ");     
  tft.fillRoundRect(120,85,40,30,2,ILI9163_ORANGE);
  tft.setTextColor(ILI9163_BLACK,ILI9163_ORANGE);   
  tft.setCursor(124,90);tft.print("Alt(m)");   
}

void printLockStatStrip(int lock) 
{
  if (lock == 1) 
  {
    tft.fillRoundRect(0,120,160,7,0,ILI9163_GREEN);    
  }
  else  
  {
    tft.fillRoundRect(0,120,160,7,0,ILI9163_RED);    
  }
}
