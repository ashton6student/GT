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

//OLED Display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

 //Initial values
int second = 0;
int minute = 0;
int hour = 0;
String hourOut, minuteOut, secondOut;

//Usage variables
int state = 0;
int counter = 0;

int currButton = 0;
int oldButton = 0;

//Setup
void setup() {
  //Serial setup
  Serial.begin(115200);
    
  //Setting I2C pins
  Wire.setPins(SDA, SCL);
  Wire.begin();

  //Display Setup
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR); //Turns on display
  display.clearDisplay(); //Clears ram buffer
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(20, 20);
  display.display();

  //Button Setup
  pinMode(buttonPin, INPUT);  
}

//Loop
void loop() {
    int buttonVal = digitalRead(buttonPin);
    stateSelect(buttonVal);
    iterateTime(hour, minute, second);
    String output = makeTime(hourOut, minuteOut, secondOut);
    // Display the result 
    display.clearDisplay(); // Clear display buffer
    display.setCursor(0, 0); // Set cursor position
    display.println(output); // Print output string
    display.display(); // Send buffer to display
    Serial.println(counter);
}

//Functions
String makeTime(const String &hour, const String &minute, const String &second) {
    String result = "";
    String newHour = "";
    String newMinute = "";
    String newSecond = "";
    if(hour.toInt() < 10 && hour != "AA") {
      newHour += "0" + hour;
    } else {
      newHour += hour;
    }
    if(minute.toInt() < 10 && minute != "AA") {
      newMinute += "0" + minute;
    } else {
      newMinute += minute;
    }
    if(second.toInt() < 10 && second != "AA") {
      newSecond += "0" + second;
    } else {
      newSecond += second;
    }

    result += newHour + ":" + newMinute + ":" + newSecond;

    return result;
}

void iterateTime(int &hour, int &minute, int &second) {
  delay(1);
  if(counter < 100) {
    counter++;
  } else {
    counter = 0;
    if(second > 60) {
      second = 0;
      minute++;
    } else {
      second++;
    }
    if(minute > 60) {
      minute = 0;
      hour++;
    }
    if(hour > 24) {
      hour = 0;
    }  
  }
}

void stateSelect(int button) {
  currButton = button;
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
      minuteOut = String(minute);
      secondOut = String(second);      
      break;
    case 2:
      hourOut   = String(hour);
      minuteOut = "AA";
      secondOut = String(second);      
      break;
    case 3:
      hourOut   = String(hour);
      minuteOut = String(minute);
      secondOut = "AA";
      break;
    default:
      hourOut   = String(hour);
      minuteOut = String(minute);
      secondOut = String(second);
      break;
  }
}
