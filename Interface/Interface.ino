#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>

// Define size
const byte ROWS = 4;
const byte COLS = 4;
volatile int red = 5;
volatile int green = 3;
volatile int blue = 4;
Servo myServo;
volatile bool sendOpen = false;
volatile bool enterPressedFlag = false;  // flag instead of direct ISR call
char comm= 0;

// Keymap
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Connect row pins to (13,6)+ A0–A1
byte rowPins[ROWS] = {13, 6, A0, A1};
// Connect col pins to A2-A3 + (1,0)
byte colPins[COLS] = {A2, A3, 1, 0};

// Create Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd_1(12, 11, 10, 9, 8, 7);
char input[4];
int enter_key = 2;

// To Stop Writing After 3 Chars In Password
bool locked[3] = {0};  // 3 places 12,13,14 
int start_col_writing = 12;

void lockCell(int col) {
  locked[col - 12] = true;
}

bool safePrint(int col, char c) {
  if (!locked[col - 12]) {
    lcd_1.setCursor(col, 1);
    lcd_1.print(c);
    return true;
  }
  return false;
}

// draw the start of the LCD
void draw_lcd() {
  lcd_1.clear();
  delay(100);
  lcd_1.print("True Pass : 123");
  lcd_1.setCursor(0, 1);
  lcd_1.print("Write Now : ");
  lcd_1.setCursor(12, 1);
  lcd_1.blink();
  for (int i = 0; i < 3; i++) {
    locked[i] = false;
  }
  digitalWrite(green, HIGH);
  digitalWrite(red, LOW);
  digitalWrite(blue, HIGH);
}

void setup() {
  Wire.begin(0x30);
  Wire.onReceive(receiveEvent); 
  lcd_1.begin(16, 2); // Set up the number of columns and rows on the LCD.
  pinMode(enter_key, INPUT);
  attachInterrupt(digitalPinToInterrupt(enter_key), [](){ enterPressedFlag = true; }, RISING);  // safe ISR
  draw_lcd();
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  input[3] = '\0';
  myServo.attach(green); // keep your original logic
  myServo.write(0);
}

void loop() {
   if (sendOpen) {
    sendOpen = false;  // reset flag
    Wire.beginTransmission(0x27);
    Wire.write('o');
    Wire.endTransmission();
   }
   if (comm == 'c') {
    draw_lcd();
    start_col_writing = 12;
    comm = 0;   // reset after handling
  }
  for (int i = 0; i < 3; i++) {
    if (start_col_writing < 15) {
      char key = keypad.waitForKey();
      safePrint(start_col_writing, key);
      lockCell(start_col_writing);
      start_col_writing++;
      input[i] = key;
      input[i+1] = '\0'; // ensure null-termination
    }
  }
  lcd_1.noBlink();

  // Handle enter safely outside ISR
  if (enterPressedFlag) {
    enterPressedFlag = false;
    if (strcmp(input, "123") == 0) {
      lcd_1.clear();   
      lcd_1.setCursor(1, 0);
      lcd_1.print("True Password!");
      lcd_1.noBlink();
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
      myServo.write(90);
      sendOpen = true;
    } else {
      lcd_1.clear();
      lcd_1.setCursor(1, 0);
      lcd_1.print("Wrong Password!");
      digitalWrite(green, LOW);
      digitalWrite(red, HIGH);
      digitalWrite(blue, LOW);
      delay(700); // shorter feedback delay
      start_col_writing = 12;
      draw_lcd();
    }
  }
}
void receiveEvent(int bytes) {
  while (Wire.available()) {
    comm = Wire.read(); // save last received byte
  }
}
