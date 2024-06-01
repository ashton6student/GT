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

#define buzzerPIN 23
//Variables
/////////////////////
//General variables
  //Peripherals - btn1 = right , btn2 = top left, btn3 = bottom left
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
  const long inv3 = 500; // Interval for task3 (0.5 second)
  const long inv4 = 100; // Interval for task4 (0.1 second)
  
  //Control
  int state = 0; //0 - Clock, 1 - Alarm, 2 - Stopwatch, 3 - Set Clock
  String tag, output;
  String hourOut, minOut, secOut;
  
//State 0 - Clock variables
  int curSec = 0;
  int curMin = 0;
  int curHour = 0;
  
//State 1 - Alarm variables
  int alrState = 0; //0 - hold state, 1 - set hour, 2 - set minute, 3 - reset second (Unused here)
  
  int alrMin = 1;
  int alrHour = 0;
  int alrSec = 0;

  bool alrSounded = false;

  String alrHourOut,alrMinOut, alrSecOut;
  
//State 2 - Stopwatch variables
  int stpMil = 0;
  int stpSec = 0;
  int stpMin = 0;
  
  int hldMil = 0;
  int hldSec = 0;
  int hldMin = 0;

//State 3 - Set Clock variables
  int setState = 1; //0 - hold state (Unused here), 1 - set hour, 2 - set minute, 3 - reset second
  int iterate = HIGH;
  
  String setHourOut, setMinOut, setSecOut;
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
  pinMode(buzzerPIN, OUTPUT);    
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();

  //Occurs every second. 
  if(currentTime - prevTime1 >= inv1) {
    //Save Current Time
    prevTime1 = currentTime;
    
    //Occurs For All States
    iterateTime(curHour, curMin, curSec);
    
    //State Dependent Functions
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
    //Save Current Time
    prevTime2 = currentTime;
    
    //Occurs For All States
    checkPeripherals();
    stateSelect();
    alrAlarm();

    //State Dependent Functions
    switch(state) {
      case 0: //Clock state
        break;
      case 1: //Alarm state
        alrStateSelect();
        setNum(alrState, alrHour, alrMin, alrSec);
        break;
      case 2: //Stopwatch state
        break;
      case 3: //Set Clock state
        setStateSelect();
        setNum(setState, curHour, curMin, curSec);
        break;
      default:
        break;
    }
  }

  //Occurs every quarter second - USED FOR: Blinking.
  if(btn2Pressed == HIGH){
    //Save Current Time
    prevTime3 = currentTime;
  } else {
      if(currentTime - prevTime3 >= inv3) {
        //Save Current Time
        prevTime3 = currentTime;

        //Occurs For All States
        
        //State Dependent Functions
        switch(state) {
          case 0: //Clock state
            break;
          case 1: //Alarm state
            blinkNum(alrState, alrHour, alrMin, alrSec, alrHourOut, alrMinOut, alrSecOut);
            break;
          case 2: //Stopwatch state
            break;
          case 3: //Set Clock state
            blinkNum(setState, curHour, curMin, curSec, setHourOut, setMinOut, setSecOut);
            break;
          default:
            break;
        }
      }  
  }
  
  // Display the result 
  displayWatch();

  //Button press noise making
  if(curBtn1 || curBtn2 || curBtn3) {
    digitalWrite(buzzerPIN, HIGH);
  } else {
    digitalWrite(buzzerPIN, LOW);
  }
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
      hourOut = alrHourOut;
      minOut  = alrMinOut;
      secOut  = alrSecOut;
      break;
    case 2: //Stopwatch state
      tag = "Stopwatch";
      hourOut = "22";
      minOut  = "22";
      secOut  = "22";
      break;
    case 3: //Set Clock state
      tag = "Set Clock";  
      hourOut = setHourOut;
      minOut  = setMinOut;
      secOut  = setSecOut;
      break;
    default:
      break;
  }

  //Clear Display
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

void stateSelect() {
  if(btn3Pressed){
    if(state == 3)
      state = 0;
    else
      state++;
  }
}

//This function takes in a state variables and iterates the number depending on the state. The state will clarify what kind of number it is (hour, min, sec)
void setNum(const int &state, int &hour, int &minute, int &second) {
    if(curBtn1 == LOW){
    iterate = HIGH;
  }
  switch(state) {
    case 1: //in hour set
      if(btn1Pressed && iterate) {
        iterateHour(hour);
        iterate = LOW; 
      }
      break;
    case 2: //in minute set
      if(btn1Pressed && iterate) {
        iterateMin(minute);
        iterate = LOW; 
      }   
      break;
    case 3: //in second set
      if(btn1Pressed && iterate) {
        second = 0;
        iterate = LOW; 
      }
      break;
    default:
      break; 
  } 
}

void blinkNum(const int &state, const int &hour, const int &minute, const int &second, String &hourStr, String &minStr, String &secStr) {
  switch(state) {
    case 1:
      if(hourStr == "  "){
        hourStr = appendZero(hour);
      } else {
        hourStr = "  ";
      }
      minStr  = appendZero(minute);
      secStr  = appendZero(second);
      break;
    case 2:
      hourStr = appendZero(hour);
      if(minStr == "  "){
        minStr = appendZero(minute);
      } else {
        minStr = "  ";
      }
      secStr  = appendZero(second); 
      break;
    case 3:
      hourStr = appendZero(hour);
      minStr  = appendZero(minute);    
      if(secStr == "  "){
        secStr = appendZero(second);
      } else {
        secStr = "  ";
      }
      break;
    default:
      hourStr = appendZero(hour);
      minStr  = appendZero(minute);
      secStr  = appendZero(second);
      break;
  }    
}
///////////////////////////////

//Utility Functions
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

void iterateMil(int &milli) {
  if(milli > 98) {
    milli = 0;
  } else {
    milli++;
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

void iterateTime2(int &minute, int &second, int &milli) {
  //Iterate second or reset second and minute
  if(milli > 98) {
    milli = 0;
    second++;
  } else {
    milli++;
  }
  //reset minute or iterate hour
  if(second > 58) {
    second = 0;
    minute++;
  }
  //reset hour
  if(minute > 58) {
    minute = 0;
  }   
}
///////////////////////////////

//State - 0 Clock Functions
///////////////////////////////
///////////////////////////////

//State - 1 Alarm Functions
///////////////////////////////
void alrStateSelect() {
  if(btn2Pressed){
    if(alrState == 2)
      alrState = 0;
    else
      alrState++;
  }
}

void alrAlarm() {
  // Check if the current time is past the alarm time
  if (curHour > alrHour || (curHour == alrHour && curMin >= alrMin)) {
    // If the alarm has not sounded yet
    if (!alrSounded) {
      digitalWrite(buzzerPIN, HIGH);
      state = 1;
      // Check if the button is pressed to stop the alarm
      if (curBtn1) {
        alrSounded = true;
        state = 0;
      }
    } else {
      // If the alarm has already sounded, turn off the buzzer
      digitalWrite(buzzerPIN, LOW);
    }
  } else {
    // If the current time is before the alarm time, ensure the buzzer is off
    digitalWrite(buzzerPIN, LOW);
  }
}
///////////////////////////////

//State - 2 Stopwatch Functions
///////////////////////////////
void stopStateSelect() {
  
}
///////////////////////////////

//State - 3 Set Clock Functions
///////////////////////////////
void setStateSelect() {
  if(btn2Pressed){
    if(setState == 3)
      setState = 1;
    else
      setState++;
  }
}
///////////////////////////////
