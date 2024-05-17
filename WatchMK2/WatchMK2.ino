#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Peripheral Mapped Address
#define OLED_ADDR   0x3C

//I2C Wire GPIO pin numbers
#define SDA 21
#define SCL 22

//Peripheral Pins
#define buttonPin 34

//Variables
int second = 0;
int minute = 0;
int hour = 0;
String hourOut, minuteOut, secondOut;

//Usage variables
int state = 0;
int counter = 0;

int currButton = 0;
int oldButton = 0;

//Timing variables
unsigned long prevTime1 = 0;
unsigned long prevTime2 = 0;
const long inv1 = 1000; // Interval for task1 (1 second)
const long inv2 = 10; // Interval for task2 (0.01 second)

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
  pinMode(buttonPin, INPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();

  if(currentTime - prevTime1 >= inv1) {
    prevTime1 = currentTime;
    iterateTime(hour, minute, second);
  }

  if(currentTime - prevTime2 >= inv2) {
    prevTime2 = currentTime;
    stateSelect();
  }
  //Generate Outputs
  String output = hourOut + ":" + minuteOut + ":" + secondOut;
  // Display the result 
  display.clearDisplay(); // Clear display buffer
  display.setCursor(0, 0); // Set cursor position
  display.println(output); // Print output string
  display.display(); // Send buffer to display

  Serial.println(output); // Print output string
}

void iterateTime(int &hour, int &minute, int &second) {
  //Iterate second or reset second and minute
  if(second > 60) {
    second = 0;
    minute++;
  } else {
    second++;
  }
  //reset minute or iterate hour
  if(minute > 60) {
    minute = 0;
    hour++;
  }
  //reset hour
  if(hour > 24) {
    hour = 0;
  }   
}

void stateSelect() {
  currButton = digitalRead(buttonPin);
  if(currButton == HIGH && oldButton == LOW){
    if(state == 3) {
      state = 0;
    } else {
      state++;
    }
  }
  oldButton = currButton;
  switch(state) {
    case 1:
      hourOut   = "AA";
      minuteOut = appendZero(minute);
      secondOut = appendZero(second);      
      break;
    case 2:
      hourOut   = appendZero(hour);
      minuteOut = "AA";
      secondOut = appendZero(second);      
      break;
    case 3:
      hourOut   = appendZero(hour);
      minuteOut = appendZero(minute);
      secondOut = "AA";
      break;
    default:
      hourOut   = appendZero(hour);
      minuteOut = appendZero(minute);
      secondOut = appendZero(second);
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
