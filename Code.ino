#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Peripheral Mapped Address
#define OLED_ADDR   0x3C

//I2C Wire GPIO pin numbers
#define SDA 21
#define SCL 22

//Oled Display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//Insert Columns 
String insertColons(const String &input) {
    String result = "";

    for (int i = 0; i < input.length(); ++i) {
        result += input[i];
        if (i < input.length() - 1) {
            result += ':';
        }
    }

    return result;
}
 //Initial values
  int counter = 1000;

//Setup
void setup() {
  //Setting Wires
  Wire.setPins(SDA, SCL);
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR); //Turns on display
  display.clearDisplay(); //Clears ram buffer
  display.setTextColor(1);
  display.setTextSize(5);
  display.setCursor(20, 20);
//  display.println("Poop");
//  display.drawPixel(10, 10, WHITE);
  display.display();
}
//
//void loop() {
//  counter--;
//  String counterStr = String(counter);
////  String output = insertColons(counterStr);
//  display.clearDisplay(); // Clear display buffer
//  display.setCursor(20, 20); // Set cursor position
//  display.println(counterStr); // Print counter
//  delay(100); // Wait for 2 seconds
//}

void loop() {
    // Original integer
//    int number = 12345;
    counter--;
    // Convert integer to String
    String input = String(counter);

    // Insert colons between characters
    String output = insertColons(input);

    // Display the result
    display.clearDisplay(); // Clear display buffer
    display.setCursor(0, 0); // Set cursor position
    display.println(output); // Print output string
    display.display(); // Send buffer to display
    
    delay(100); // Wait for 5 seconds before repeating
}
