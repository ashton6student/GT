#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Peripheral Mapped Address
#define OLED_ADDR   0x3C

//I2C Wire GPIO pin numbers
#define SDA 21
#define SCL 22

//Peripheral Pins
#define btn1PIN 33
#define btn2PIN 32
#define btn3PIN 35

//Variables
/////////////////////
//General variables
  //Peripherals
  int curBtn1 = 0;
  int oldBtn1 = 0;
  int btn1Pressed;

  int curBtn2 = 0;
  int oldBtn2 = 0;
  int btn2Pressed;

  int curBtn3 = 0;
  int oldBtn3 = 0;
  int btn3Pressed;

  //Timing variables
  unsigned long prevTime1 = 0;
  unsigned long prevTime2 = 0;
  unsigned long prevTime3 = 0;
  
  const long inv1 = 1000; // Interval for task1 (1 second)
  const long inv2 = 10; // Interval for task2 (0.01 second)
  const long inv3 = 500; // Interval for task2 (0.5 second)
  
  //Control
  int state = 0; //0 - Clock, 1 - Alarm, 2 - Stopwatch, 3 - Set Clock
  
//State 0 - Clock variables
  int curSec = 0;
  int curMin = 0;
  int curHour = 0;

  String hourOut, minOut, secOut, tag;
  
//State 1 - Alarm variables
  int alrMin = 0;
  int alrHour = 0;
  
//State 2 - Stopwatch variables
  int stpMil = 0;
  int stpSec = 0;
  int stpMin = 0;

//State 3 - Set Clock variables
  int setState; //0 - Don't set, 1 - set hour, 2 - set minute, 3 - reset second
  int iterate = HIGH;
  String hourBlink, minBlink, secBlink;
/////////////////////

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

  //Peripheral Setup
  pinMode(btn1PIN, INPUT);
  pinMode(btn2PIN, INPUT);
  pinMode(btn3PIN, INPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();

  //Occurs every second. 
  if(currentTime - prevTime1 >= inv1) {
    //Save current time
    prevTime1 = currentTime;
    //Occurs For All States
    iterateTime(curHour, curMin, curSec);
    //State Select
    switch(state) {
      case 0: //Clock state
        break;
      case 1: //Alarm state
        break;
      case 2: //Stopwatch state
        break;
      case 3: //Set Clock state
        break;
      default:
        break;
    }
  }

  //Occurs every 0.01 seconds - USED FOR: Setting setState, setting display numbers.
  if(currentTime - prevTime2 >= inv2) {
    prevTime2 = currentTime;
    
    //State Select
    checkPeripherals();
    stateSelect();
    switch(state) {
      case 0: //Clock state
        break;
      case 1: //Alarm state
        break;
      case 2: //Stopwatch state
        break;
      case 3: //Set Clock state
        setStateSelect();
        setNum();
        break;
      default:
        break;
    }
  }

  //Occurs every quarter second - USED FOR: Blinking in Set Clock state.
  if(btn1Pressed == HIGH){
    prevTime3 = currentTime;
  } else {
      if(currentTime - prevTime3 >= inv3) {
        prevTime3 = currentTime;
      //State Select
      switch(state) {
        case 0: //Clock state
          break;
        case 1: //Alarm state
          break;
        case 2: //Stopwatch state
          break;
        case 3: //Set Clock state
          blinkNum();
          break;
        default:
          break;
      }
    }  
  }
  // Display the result 
  displayWatch();
}

//General Functions
///////////////////////////////
void displayWatch() {
  switch(state) {
    case 0: //Clock state
      tag = "Clock";
      hourOut = appendZero(curHour);
      minOut  = appendZero(curMin);
      secOut  = appendZero(curSec);
      break;
    case 1: //Alarm state
      tag = "Alarm";
      hourOut = "11";
      minOut  = "11";
      secOut  = "11";
      break;
    case 2: //Stopwatch state
      tag = "Stopwatch";
      hourOut = "22";
      minOut  = "22";
      secOut  = "22";
      break;
    case 3: //Set Clock state
      tag = "Set Clock";
        switch(setState) {
          case 0:
            hourOut = hourBlink;
            minOut  = appendZero(curMin);
            secOut  = appendZero(curSec);      
            break;
          case 1:
            hourOut = appendZero(curHour);
            minOut  = minBlink;
            secOut  = appendZero(curSec);      
            break;
          case 2:
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
      break;
    default:
      break;
  }
  String output = hourOut + ":" + minOut + ":" + secOut;
  display.clearDisplay(); // Clear display buffer

  //Draw Tag
  display.setCursor(0, 0); // Set cursor position
  display.setTextSize(1);
  display.println(tag); // Print output string

  //Draw Date
  display.setCursor(70, 0); // Set cursor position
  display.setTextSize(1);
  display.print("5/18/2024"); // Print output string  
  
  //Draw First Set of Numbers
  display.setCursor(0, 40); // Set cursor position
  display.setTextSize(3);
  display.print(hourOut + ":" + minOut); // Print output string

  //Draw Second Set of Numbers
//  display.setCursor(0, 35); // Set cursor position
  display.setTextSize(2);
  display.println(":" + secOut); // Print output string
  display.display(); // Send buffer to display
  Serial.println("btn1: " + String(digitalRead(btn1PIN)) + " btn2: " + String(digitalRead(btn2PIN)) + " btn3: " + String(digitalRead(btn3PIN))); // Print output string
  Serial.println(state); // Print output string  
}

void checkPeripherals() {
  curBtn1 = digitalRead(btn1PIN);
  curBtn2 = digitalRead(btn2PIN);
  curBtn3 = digitalRead(btn3PIN);
  
  if(curBtn1 == HIGH && oldBtn1 == LOW)
    btn1Pressed = HIGH;
  else
    btn1Pressed = LOW;
    
  if(curBtn2 == HIGH && oldBtn2 == LOW)
    btn2Pressed = HIGH;
  else
    btn2Pressed = LOW;
    
  if(curBtn3 == HIGH && oldBtn3 == LOW)
    btn3Pressed = HIGH;
  else
    btn3Pressed = LOW;
    
  oldBtn1 = curBtn1; 
  oldBtn2 = curBtn2; 
  oldBtn3 = curBtn3;  
}
///////////////////////////////

//State - 0 Clock Functions
///////////////////////////////
void stateSelect() {
  if(btn3Pressed){
    if(state == 3)
      state = 0;
    else
      state++;
  }
}
///////////////////////////////

//State - 1 Alarm Functions
///////////////////////////////
///////////////////////////////

//State - 2 Stopwatch Functions
///////////////////////////////
///////////////////////////////

//State - 3 Set Clock Functions
///////////////////////////////
void setStateSelect() {
  if(btn1Pressed){
    if(setState == 2)
      setState = 0;
    else
      setState++;
  }
}

void setNum() {
  if(curBtn2 == LOW){
    iterate = HIGH;
  }
  switch(setState) {
    case 0:
      if(btn2Pressed && iterate) {
        iterateHour(curHour);
        iterate = LOW; 
      }
      break;
    case 1:
      if(btn2Pressed && iterate) {
        iterateMin(curMin);
        iterate = LOW; 
      }   
      break;
    case 2:
      if(btn2Pressed && iterate) {
        curSec = 0;
        iterate = LOW; 
      }
      break;
    default:
      break; 
  } 
}

void blinkNum() {
  switch(setState) {
    case 0:
      if(hourBlink == "  "){
        hourBlink = appendZero(curHour);
      } else {
        hourBlink = "  ";
      }
      minBlink  = "  ";
      secBlink  = "  ";
      break;
    case 1:
      hourBlink = "  ";
      if(minBlink == "  "){
        minBlink = appendZero(curMin);
      } else {
        minBlink = "  ";
      }
      secBlink  = "  "; 
      break;
    case 2:
      hourBlink = "  ";
      minBlink  = "  ";    
      if(secBlink == "  "){
        secBlink = appendZero(curSec);
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
///////////////////////////////

//General Utility Functions
///////////////////////////////
String appendZero(const int &num) {
  String result = "";
  if(num < 10){
    result += "0" + String(num);
  } else {
    result += String(num);
  }

  return result;
}

void iterateHour(int &hour) {
  //reset hour
  if(hour > 22) {
    hour = 0;
  } else {
    hour++;  
  }
}

void iterateMin(int &minute) {
  //reset minute or iterate hour
  if(minute > 58) {
    minute = 0;
  } else {
    minute++;  
  }
}

void iterateSec(int &second) {
  if(second > 58) {
    second = 0;
  } else {
    second++;
  }  
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
///////////////////////////////
