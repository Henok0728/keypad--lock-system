#include <arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const byte ROWS = 4;
const byte COLS = 3;
int trial = 0;
const int max_trial = 3;
int greenpin = 11 ;
int redpin = 12;
Servo door;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {5,4,3,2};
byte colPins[COLS] = {8,9,10};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
volatile String input = "";
String password = "1234";
const int length = 4;
const String sets = "1234567890";
const String generatePassword(int length,String sets) {
  String pwd = "";
  int charsetSize = sizeof(sets) - 1;
  for (int i = 0; i < length; i++) {
    int index = random(charsetSize);
    pwd += sets[index];
  }
  return pwd;
}
void setup() {
  randomSeed(analogRead(A0));
  door.attach(6);
  pinMode(greenpin,OUTPUT);
  pinMode(redpin,OUTPUT);
  door.write(0);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); // halt if OLED not found
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Loading...");
  display.display();
  delay(1000);
  display.clearDisplay();

}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Security System");
  display.println("ENTER YOUR PASSWORD:");
  display.setCursor(0, 20);
  for (int i = 0; i < input.length(); i++) {
    display.print("*");   
  }
  display.display();
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {  
      if (input == password) {
        password = "";
        password = generatePassword(length, sets);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("ACCESS GRANTED");
        display.print("your new password is ");
        display.print(password);
        display.display();
        digitalWrite(greenpin,HIGH);
        for (int pos = 0; pos <= 180; pos += 1) { 
   
          door.write(pos);             
          delay(15);                      
        }
        delay(4000);
        for (int pos = 110; pos >= 0; pos -= 1) { 
          door.write(pos);              
          delay(15);                     
        }
       
        input = "";
        digitalWrite(greenpin,LOW);
      } 
      else {
        trial++;
        display.clearDisplay();
        display.setCursor(0, 0);
        if (trial < max_trial) {
          display.println("Wrong password!");
          display.print("Trials left: ");
          display.println(max_trial - trial);
          display.display();
          delay(2000);
          input = ""; 
        } 
        else {
          door.write(0);
          display.println("ACCESS DENIED");
          display.display();
          digitalWrite(redpin,HIGH);
          while (true); 
        }
      }
    } 
    else if (key == '*') {  
      input = "";
    } 
    else {  
      input += key;
    }
  }
}

