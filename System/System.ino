#include <Adafruit_LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment display = Adafruit_7segment();
int counter = 0;
int down = 2;
int up = 3;
int enter = 4;
int motor = A3;
int l_1 = 0;
int l_2 = 1;
int l_3 = 5;
int l_4 = 6;
// LED 1 
int r_1 = 8;
int g_1 = 9;
// LED 2 
int r_2 = 10;
int g_2 = 11;
// LED 3 
int r_3 = 12;
int g_3 = 13;  

Adafruit_LiquidCrystal lcd_1(0);
volatile char command = 0;  // store received command
volatile bool status_d = false;
volatile bool status_u = false;
volatile bool re = false;
volatile bool looop = false;
bool sendclose = false;
volatile int flor = 1;
void go_down() {
  status_d = true;
  looop = true; 
  counter = 0;
  re = true;
}

void reset_c() {
  display.clear();
  display.writeDisplay();
  analogWrite(motor, 0);
    digitalWrite(r_1, LOW);   
    digitalWrite(g_1, LOW);
    digitalWrite(r_2, LOW);   
  digitalWrite(r_3, LOW);
  digitalWrite(g_2, LOW);
  digitalWrite(g_3, LOW);
}

void go_up() {
  status_u = true;
  looop = true;
  counter = 0;
  re = true;
}

void setup() {
  display.begin(0x70); 
  pinMode(down, INPUT);   // button on pin 2
  pinMode(up, INPUT);     // button on pin 3
  pinMode(enter, INPUT);
  pinMode(l_1, INPUT_PULLUP);
  pinMode(l_2, INPUT_PULLUP);
  pinMode(l_3, INPUT_PULLUP);
  pinMode(l_4, INPUT_PULLUP);
  pinMode(motor, OUTPUT);
  pinMode(r_1, OUTPUT);
  pinMode(g_1, OUTPUT);
  pinMode(r_2, OUTPUT);
  pinMode(g_2, OUTPUT);
  pinMode(r_3, OUTPUT);
  pinMode(g_3, OUTPUT);
  lcd_1.begin(16, 2);
  lcd_1.setBacklight(0);
  Wire.begin(0x27);               // join I2C bus as slave with address 0x27
  Wire.onReceive(receiveEvent);   // register event callback
  attachInterrupt(digitalPinToInterrupt(down), go_down, RISING); 
  attachInterrupt(digitalPinToInterrupt(up), go_up, RISING);
  Serial.begin(9600);
}

void loop() {
  if (command == 'o') {
    s_1();
    command = 0;   // reset after handling
  }
   if (sendclose) {
     Serial.print(1);
    sendclose = false;  // reset flag
    Wire.beginTransmission(0x30);
    Wire.write('c');
    Wire.endTransmission();
   }

  if (looop) {
    if (flor == 2 && status_d) {
      status_u = false;
      status_d = false;
      flor += 1;
      s_2();
    }

    if (flor == 3 && status_u) {
      status_u = false;
      status_d = false;
      flor -= 1;
      s_3();  
    }

    if (status_d) {
      status_d = false;
      lcd_1.setCursor(0, 0);
      lcd_1.print(" ");
      lcd_1.setCursor(0, 1);
      lcd_1.print(">");
      flor++;
    }

    if (status_u) {
      status_u = false;
      lcd_1.setCursor(0, 1);
      lcd_1.print(" ");
      lcd_1.setCursor(0, 0);
      lcd_1.print(">");
      flor--;
    }

    looop = false;
  }

  if (digitalRead(enter) == HIGH) { 
    
    if (flor == 1) {
      display.print(counter);   // Print number
      display.writeDisplay();   // Push to display
      counter++;
      delay(50);
      re = false;  
    } 
    else if (flor == 2) {
      reset_c();
      if (digitalRead(l_1) == LOW || digitalRead(l_2) == LOW || digitalRead(l_3) == LOW || digitalRead(l_4) == LOW) {
        if (digitalRead(l_4) == LOW) { analogWrite(motor, 255); }
        else if (digitalRead(l_3) == LOW) { analogWrite(motor, 200); }
        else if (digitalRead(l_2) == LOW) { analogWrite(motor, 190); }
        else if (digitalRead(l_1) == LOW) { analogWrite(motor, 180); }
      } else {
        analogWrite(motor, 0); // motor off if no switch is pressed
      }    
    }
    else if (flor == 3) {
     // ----- Light 1 Cycle -----
digitalWrite(r_1, HIGH);   // Red ON
digitalWrite(g_1, LOW);
digitalWrite(r_2, HIGH);   // Others Red
digitalWrite(r_3, HIGH);
digitalWrite(g_2, LOW);
digitalWrite(g_3, LOW);
delay(500);

digitalWrite(r_1, HIGH);   // Yellow = Red + Green
digitalWrite(g_1, HIGH);
delay(500);

digitalWrite(r_1, LOW);    // Green ON
digitalWrite(g_1, HIGH);
delay(1000);

digitalWrite(g_1, LOW);    // All off before next cycle

// ----- Light 2 Cycle -----
digitalWrite(r_2, HIGH);   // Red ON
digitalWrite(g_2, LOW);
digitalWrite(r_1, HIGH);   // Others Red
digitalWrite(r_3, HIGH);
digitalWrite(g_1, LOW);
digitalWrite(g_3, LOW);
delay(500);

digitalWrite(r_2, HIGH);   // Yellow = Red + Green
digitalWrite(g_2, HIGH);
delay(500);

digitalWrite(r_2, LOW);    // Green ON
digitalWrite(g_2, HIGH);
delay(1000);

digitalWrite(g_2, LOW);    // All off before next cycle

// ----- Light 3 Cycle -----
digitalWrite(r_3, HIGH);   // Red ON
digitalWrite(g_3, LOW);
digitalWrite(r_1, HIGH);   // Others Red
digitalWrite(r_2, HIGH);
digitalWrite(g_1, LOW);
digitalWrite(g_2, LOW);
delay(500);

digitalWrite(r_3, HIGH);   // Yellow = Red + Green
digitalWrite(g_3, HIGH);
delay(500);

digitalWrite(r_3, LOW);    // Green ON
digitalWrite(g_3, HIGH);
delay(1000);

digitalWrite(g_3, LOW);    // All off before next cycle

    }
   else if (flor == 4)  // new condition added
{
    lcd_1.clear();
    lcd_1.setBacklight(0);
    sendclose = true;
    reset_c();
    flor=1;
}
  }

  if (re) {
    reset_c();
  }
}

void s_1() {
  lcd_1.setBacklight(1);
  lcd_1.clear();
  lcd_1.setCursor(4, 0);
  lcd_1.print("Welcome :)");
  delay(150);
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Right B : Down");
  lcd_1.setCursor(0, 1);
  lcd_1.print("Left B : Up");
  delay(150);
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Switch :Choose");
  delay(150);
  lcd_1.clear();
  lcd_1.setCursor(1, 0);
  lcd_1.print("(1 Stop Watch");
  lcd_1.setCursor(1, 1);
  lcd_1.print("(2 Motor");
  lcd_1.setCursor(0, 0);
  lcd_1.print(">");
}

void s_2() {
  lcd_1.clear();
  lcd_1.setCursor(1, 0);
  lcd_1.print("(3 Traffic");
  lcd_1.setCursor(1, 1);
  lcd_1.print("(4 End");
  lcd_1.setCursor(0, 0);
  lcd_1.print(">");
}

void s_3() {
  lcd_1.clear();
  lcd_1.setCursor(1, 0);
  lcd_1.print("(1 Stop Watch");
  lcd_1.setCursor(1, 1);
  lcd_1.print("(2 Time");
  lcd_1.setCursor(0, 1);
  lcd_1.print(">");
}

void receiveEvent(int bytes) {
  while (Wire.available()) {
    command = Wire.read(); // save last received byte
  }
}
