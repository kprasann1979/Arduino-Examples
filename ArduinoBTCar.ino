//Choose option Arduino Pro or Pro Mini if you are uploading to the box car
#include <SoftwareSerial.h>
#include <NewPing.h>
#include <Adafruit_NeoPixel.h>
#define TRIGGER_PIN  2
#define ECHO_PIN     3
#define MAX_DISTANCE 200
#define N_LEDS 5
#define OBSTACLE_THRESHOLD 15
SoftwareSerial btSerial(A3,A2); 
const int motorA1 = 5;
const int motorA2 = 6;
const int motorB1 = 7;
const int motorB2 = 8;
const int led = A4;
const int neoLED = A5;
const int buzzer = 4;
unsigned long previousMillis = 0;
const long interval = 10000;
int stateVal = 0;

int dist = 0;int distR = 0;int distL = 0;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, neoLED, NEO_GRB + NEO_KHZ800);

void setup(){
  strip.begin();
  strip.setBrightness(32);  
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(buzzer, OUTPUT); 
  pinMode(led, OUTPUT);  
  pinMode(neoLED, OUTPUT);  
  btSerial.begin(9600);
  Serial.begin(9600);
}
 
void loop(){
  if(btSerial.available() > 0){     
    stateVal = btSerial.read();
  }
  if (stateVal == 'X'){
    demoMode();
  }
  else if (stateVal == 'F'){
    moveForward();    
  }      
  else if (stateVal == 'B'){
    moveBack();
  }  
  else if (stateVal == 'L'){
    moveLeft();
  }
  else if (stateVal == 'R'){
    moveRight();
  }
  else if (stateVal == 'W') {
    setNeoPixelOn();
  }  
  else if (stateVal == 'w') {
    setNeoPixelOff();
  }    
  else if (stateVal == 'V'){
    setBuzzerOn(); 
  }  
  else if (stateVal == 'v'){
    setBuzzerOff(); 
  }    
  else if (stateVal == 'U'){
    setLED_On();
  }    
  else if (stateVal == 'u'){
    setLED_Off();
  }      
  else if (stateVal == 'S'){
    stopAll();
  }
}

void moveForward(){
  digitalWrite(motorA1, HIGH); digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH); digitalWrite(motorB2, LOW);    
}
void moveBack(){
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW); digitalWrite(motorB2, HIGH);    
}
void moveRight(){
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, LOW);  
  digitalWrite(motorB1, HIGH);    digitalWrite(motorB2, LOW);    
}
void moveLeft(){
  digitalWrite(motorA1, HIGH); digitalWrite(motorA2, LOW);  
  digitalWrite(motorB1, LOW);    digitalWrite(motorB2, LOW);    
} 
void stopAll(){
  digitalWrite(motorA1, LOW);  digitalWrite(motorA2, LOW); 
  digitalWrite(motorB1, LOW);  digitalWrite(motorB2, LOW);   
} 

void setBuzzerOn()
{
  tone(buzzer, 1000); 
  stateVal='n';   
}
void setBuzzerOff()
{
  noTone(buzzer); 
  stateVal='n';   
}

void setLED_On()
{
  digitalWrite(led, HIGH);
}
void setLED_Off()
{
  digitalWrite(led, LOW);
}

void setNeoPixelOn()
{
  int j=0;
  for(j=0; j<= 4; j++) strip.setPixelColor(j, 0x40e0d0);
  strip.show();
}
void setNeoPixelOff()
{
  int j=0;
  for(j=0; j<= 4; j++) strip.setPixelColor(j, 0);
  strip.show();
}

void demoMode()
{
  unsigned long currentMillis = millis();
  while (millis() - currentMillis <= interval)
  {
    dist = 0;distR = 0;distL = 0;
    dist = pingSensor();
    Serial.print("dist=");Serial.println(dist);
    if (dist < OBSTACLE_THRESHOLD)
    {
      stopAll();
      moveBack();
      delay(1000);
      stopAll();
      moveRight();
      delay(1000);
      stopAll();
      distR = pingSensor();
      moveLeft();
      delay(1000);
      stopAll();
      distL = pingSensor();
      if (distR > distL)
      {
        moveRight();
        delay(1000);
        stopAll();
      }
      else
      {
        moveLeft();
        delay(1000);
        stopAll();      
      }
    }
    moveForward();
  }
}

int pingSensor()
{
  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  return sonar.ping_cm();
}
