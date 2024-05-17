#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Peripheral Mapped Address
#define OLED_ADDR   0x3C

//I2C Wire GPIO pin numbers
#define SDA 21
#define SCL 22

//Peripheral Pins
#define btn1PIN 34
#define btn2PIN 35

//Variables
int curSec = 0;
int curMin = 0;
int curHour = 0;

int setSec = 0;
int setMin = 0;
int setHour = 0;

String hourOut, minOut, secOut;
String hourBlink, minBlink, secBlink;

//Usage variables
int state = 0;
int newState = LOW;
int counter = 0;

int curBtn1 = 0;
int oldBtn1 = 0;

int curBtn2 = 0;
int oldBtn2 = 0;

//Timing variables
unsigned long prevTime1 = 0;
unsigned long prevTime2 = 0;
unsigned long prevTime3 = 0;
const long inv1 = 1; // Interval for task1 (1 second)
const long inv2 = 10; // Interval for task2 (0.01 second)
const long inv3 = 500; // Interval for task2 (0.5 second)

//OLED Display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  //Serial setup
  Serial.begin(115200);
    
  //Setting I2C pins
  Wire.setPins(SDA, SCL);
  Wire.begin();

  //Display Setup
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR); //Turns on display
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(20, 20);

  //Peripheral Setup
  pinMode(btn1PIN, INPUT);
  pinMode(btn2PIN, INPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();

  if(currentTime - prevTime1 >= inv1) {
    prevTime1 = currentTime;
    iterateTime(curHour, curMin, curSec);
  }

  if(currentTime - prevTime2 >= inv2) {
    prevTime2 = currentTime;
    stateSelect();
    if(newState) {
      curHour = setHour;
      curMin = setMin;
      curSec = setSec;  
    } else {
      setHour = curHour;
      setMin = curMin;
      setSec = curSec;
    }
    setNum();
  }

  if(newState == HIGH){
    prevTime3 = currentTime;
  } else {
      if(currentTime - prevTime3 >= inv3) {
        prevTime3 = currentTime;
        blinkNum();
      }  
  }
  
  //Generate Outputs
  String output = hourOut + ":" + minOut + ":" + secOut;
  
  // Display the result 
  display.clearDisplay(); // Clear display buffer
  display.setCursor(10, 30); // Set cursor position
  display.println(output); // Print output string
  display.display(); // Send buffer to display
  Serial.println(digitalRead(btn2PIN)); // Print output string
}

void iterateTime(int &hour, int &minute, int &second) {
  //Iterate second or reset second and minute
  if(second > 58) {
    second = 0;
    minute++;
  } else {
    second++;
  }
  //reset minute or iterate hour
  if(minute > 58) {
    minute = 0;
    hour++;
  }
  //reset hour
  if(hour > 22) {
    hour = 0;
  }   
}

void stateSelect() {
  curBtn1 = digitalRead(btn1PIN);
  if(curBtn1 == HIGH && oldBtn1 == LOW){
    newState = HIGH;
    if(state == 3) {
      state = 0;
    } else {
      state++;
    }
  } else {
    newState = LOW;
  }
  oldBtn1 = curBtn1;
  switch(state) {
    case 1:
      hourOut = hourBlink;
      minOut  = appendZero(curMin);
      secOut  = appendZero(curSec);      
      break;
    case 2:
      hourOut = appendZero(curHour);
      minOut  = minBlink;
      secOut  = appendZero(curSec);      
      break;
    case 3:
      hourOut = appendZero(curHour);
      minOut  = appendZero(curMin);
      secOut  = secBlink;
      break;
    default:
      hourOut = appendZero(curHour);
      minOut  = appendZero(curMin);
      secOut  = appendZero(curSec);
      break;
  }  
}

void setNum() {
  curBtn2 = digitalRead(btn2PIN);
  if(curBtn2 == HIGH && oldBtn2 == LOW){
    btn2Pressed = HIGH;
  } else {
    btn2Pressed = LOW;
  }
  switch(state) {
    case 1:
      if(btn2Pressed) {
        iterateNum(setHour); 
      }
      break;
    case 2:
      if(btn2Pressed) {
        iterateNum(setMin); 
      }   
      break;
    case 3:
      if(btn2Pressed) {
        setSec = 0; 
      }
      break;
    default:
      break;  
}

void blinkNum() {
  setSec = curSec;
  switch(state) {
    case 1:
      if(hourBlink == "  "){
        hourBlink = appendZero(setHour);
      } else {
        hourBlink = "  ";
      }
      minBlink  = "  ";
      secBlink  = "  ";
      break;
    case 2:
      hourBlink = "  ";
      if(minBlink == "  "){
        minBlink = appendZero(setMin);
      } else {
        minBlink = "  ";
      }
      secBlink  = "  "; 
      break;
    case 3:
      hourBlink = "  ";
      minBlink  = "  ";    
      if(secBlink == "  "){
        secBlink = appendZero(setSec);
      } else {
        secBlink = "  ";
      }
      break;
    default:
      hourBlink = "  ";
      minBlink  = "  ";
      secBlink  = "  ";
      break;
  }    
}

String appendZero(const int &num) {
  String result = "";
  if(num < 10){
    result += "0" + String(num);
  } else {
    result += String(num);
  }

  return result;
}
