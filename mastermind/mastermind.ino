// Include all required libraries here
#include <Wire.h>
#include <LiquidCrystal_I2C.h>                  //https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <Keypad.h>                             //https://github.com/Chris--A/Keypad
#include <ezBuzzer.h>                           //https://github.com/ArduinoGetStarted/buzzer
#include <Arduino.h>
#include <TM1637Display.h>                      //https://github.com/avishorp/TM1637

// Constant variables definitions
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
const int COLUMN_LCD = 20;
const int ROW_LCD = 4;

// Module connection pins (Digital Pins)
#define CLK 11
#define DIO 10
const int BUZZER_PIN = 12;
byte pin_rows[ROW_NUM] = {9, 8, 7, 6};      //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

// All instance definitions
TM1637Display display(CLK, DIO);
ezBuzzer buzzer(BUZZER_PIN);
LiquidCrystal_I2C lcd(0x27, COLUMN_LCD, ROW_LCD);
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// All variable declarations
int melody_1[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};
int melody_2[] = {
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5
};
int noteDurations_1[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};
int noteDurations_2[] = {
  8, 8, 8, 16, 16
};
int noteLength_1;
int noteLength_2;
int keyCount = 0;
int tryCount = 0;
int cPos=0;
int rPos = 0;
int randNumber = 0;
String randStr;
String guessNum = "";
String scrollingMessage = "";
bool firstEntry = false;// Used to clear LCD for the first time after keypress A
bool gameState = false; // Used to track status of game i.e. won or lost
bool gameStart = false; // Used to enforce keypress A once powered on or game completes or game lost

// All custom character definitions declaration
byte customChar0c1b[] = {B00000,B00000,B11100,B00000,B00000,B00000,B00000,B00000};
byte customChar0c2b[] = {B00000,B00000,B01110,B00000,B01110,B00000,B00000,B00000};
byte customChar1c2b[] = {B11111,B00000,B11100,B00000,B11100,B00000,B00000,B00000};
byte customChar1c3b[] = {B11111,B00000,B11100,B00000,B11100,B00000,B11100,B00000};
byte customChar2c1b[] = {B11111,B00000,B11111,B00000,B11100,B00000,B00000,B00000};
byte customChar2c2b[] = {B11111,B00000,B11111,B00000,B11100,B00000,B11100,B00000};
byte customChar3c0b[] = {B11111,B00000,B11111,B00000,B11111,B00000,B00000,B00000};
byte customChar3c1b[] = {B11111,B00000,B11111,B00000,B11111,B00000,B11100,B00000};

void createCustomCharacters(){
  lcd.createChar(0,customChar0c1b);
  lcd.createChar(1,customChar0c2b);
  lcd.createChar(2,customChar1c2b);
  lcd.createChar(3,customChar1c3b);
  lcd.createChar(4,customChar2c1b);
  lcd.createChar(5,customChar2c2b);
  lcd.createChar(6,customChar3c0b);
  lcd.createChar(7,customChar3c1b);
}

void generate_random_number()
{
  int d1,d2,d3,d4 = 0;
  d1 = random(0,10);
  d2 = random(0,10);
  while (d1 == d2){
    d2 = random(0,10); 
  }
  d3 = random(0,10);
  while ((d1 == d2) or (d1 == d3) or (d2 == d3)){
    d3 = random(0,10);
  }
  d4 = random(0,10);
  while ((d1 == d2) or (d1 == d3) or (d2 == d3) or (d3 == d4) or (d1 == d4) or (d2 == d4)){
    d4 = random(0,10);
  } 
  randNumber = (d1*1000)+(d2*100)+(d3*10)+d4;
  randStr = String(d1)+String(d2)+String(d3)+String(d4);
  dispGameStart();
}

void dispGameStart(){
  lcd.clear();
  display.showNumberDec(randNumber, true);
  lcd.setCursor(0,0);lcd.print("Secret code created,");
  lcd.setCursor(0,1);lcd.print("Wait for the buzzer!");
  if (buzzer.getState() == BUZZER_IDLE) { // if stopped
    buzzer.playMelody(melody_1, noteDurations_1, noteLength_1); // playing
  }
  lcd.setCursor(0,2);lcd.print("==Ready to play!!!==");
  lcd.setCursor(0,3);lcd.print("_ is Cows, ");
  lcd.setCursor(11,3);lcd.write(0);
  lcd.setCursor(13,3);lcd.print("is Bull");  
}

void scrollMessage(int startPos, int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(startPos, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}

void checkNum(String strVal){
  int indexPos = -1;
  int matchNumPos = 0;int matchNum = 0;
  for (int i=0;i<4;i++){
    indexPos = randStr.indexOf(strVal.substring(i,i+1));
    if (indexPos != -1){
      if (indexPos == i){
        matchNumPos++;
      }
      else{
        matchNum++;
      }
    }
  }
  switch (matchNumPos){
    case 0:
      if (matchNum == 0){
        lcd.print("*");
      }
      else if (matchNum == 1){
        lcd.write(0);
      }
      else if (matchNum == 2){
        lcd.write(1);
      }
      else if (matchNum == 3){
        lcd.write(0xD0);
      }
      else if (matchNum == 4){
        lcd.write(0xEF);
      }      
      break;
    case 1:
      if (matchNum == 0){
        lcd.print("_");
      }
      else if (matchNum == 1){
        lcd.write(0xC6);
      }
      else if (matchNum == 2){
        lcd.write(2);
      }
      else if (matchNum == 3){
        lcd.write(3);
      }
      break;
    case 2:
      if (matchNum == 0){
        lcd.print("=");
      }
      else if (matchNum == 1){
        lcd.write(4);
      }
      else if (matchNum == 2){
        lcd.write(5);
      }
      break;
    case 3:
      if (matchNum == 0){
        lcd.write(6);
      }
      else if (matchNum == 1){
        lcd.write(7);
      }
      break;
    case 4:
      if (matchNum == 0){
        lcd.write(0xF5);
        lcd.setCursor(16,3);lcd.print("WIN!");
        if (buzzer.getState() == BUZZER_IDLE) { // if stopped
          buzzer.playMelody(melody_2, noteDurations_2, noteLength_2); // playing
        }
        gameState = true;
        gameStart=false;
      }
      break;
  }
  if (rPos == 3){
    rPos = 0;
    cPos = cPos+5;
  }
  else{
    rPos++;
  }
  lcd.setCursor(cPos,rPos);
}

void initVars(){
  firstEntry = true;
  keyCount = 0;
  tryCount=0;
  rPos=0;cPos=0;
  gameState=false;
  gameStart=true;  
}

void dispInfo(){
  lcd.setCursor(16,3);lcd.print("E:NA");
  delay(500);
  lcd.setCursor(16,3);lcd.print("    ");
  buzzer.beep(500);
  lcd.setCursor(cPos+keyCount,rPos);  
}

void setup()  
{
  Serial.begin(9600);
  noteLength_1 = sizeof(noteDurations_1) / sizeof(int);
  noteLength_2 = sizeof(noteDurations_2) / sizeof(int);
  display.setBrightness(0x0f);
  display.clear();
  lcd.begin();
  lcd.clear();
  lcd.blink();
  randomSeed(analogRead(0));
}

void loop()
{
  buzzer.loop(); // MUST call the buzzer.loop() function in loop()
  char key = keypad.getKey();
  switch (key){
    case 'A':
      createCustomCharacters();
      generate_random_number();
      initVars();
      break;
    case '#':
      if (gameStart){
        if (!gameState){
          if (keyCount < 4) {
            dispInfo();
          }
          else{
            keyCount = 0;
            checkNum(guessNum);
            guessNum = "";
            tryCount++;
            if ((tryCount >= 15) and (!gameState)) {
              scrollingMessage = "LOST! Press A to start game...";
              for (int i=0;i<3;i++){
                scrollMessage(16, 3, scrollingMessage, 250, 4);
              }
              gameStart=false;
            }
          }
        }
      }
      else{
        lcd.clear();lcd.print("Press A to start...");
      }      
      break;      
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      buzzer.beep(100); // generates a 100ms beep
      if (gameStart){
        if (!gameState){
          keyCount++;
          if (firstEntry){
            lcd.clear();lcd.setCursor(0,0);
            firstEntry = false;
          }
          if (keyCount > 4){
            keyCount=4;
            dispInfo();
          }
          else{
            lcd.print(key);
            guessNum = guessNum + key;
          }
        }
      }
      else{
        lcd.clear();lcd.print("Press A to start...");
      }
  }
}
