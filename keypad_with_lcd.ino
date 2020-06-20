#include <Keypad.h>
#include<EEPROM.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C
#define GRN 4
#define RED 3
#define BUZ 5

SSD1306AsciiWire oled;

char password[4];
char initial_password[4], new_password[4];
int i = 0;
int stroke = 0;
char key_pressed = 0;
const byte rows = 4;
const byte columns = 4;

char hexaKeys[rows][columns] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte row_pins[rows] = {12,11,10,9};
byte column_pins[columns] = {8,7,6};

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup() {
  Wire.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();
  oled.setFont(System5x7);
  Serial.begin(9600);
  pinMode(BUZ, OUTPUT);
  digitalWrite(BUZ, LOW);
  pinMode(GRN, OUTPUT);
  digitalWrite(GRN, LOW);
  pinMode(RED, OUTPUT);
  digitalWrite(RED, LOW);
  oled.clear();
  oled.set1X();
  initialpassword();
  oled.print("Podaj kod: ");
  oled.set2X();
  oled.setCursor(13, 3);
  oled.print("____");
  oled.set1X();
  Serial.println("boop");
}

void loop() {
  key_pressed = keypad_key.getKey();
  if (key_pressed == '#')
    change();
  if (key_pressed) {
    Serial.println(key_pressed);
    beepKey();
    stroke++;
    oled.setCursor(stroke * 13, 3);
    oled.set2X();
    password[i++] = key_pressed;
    oled.print(key_pressed);
    oled.set1X();
  }

  if (i == 4) {
    stroke = 0;
    delay(200);
    for (int j = 0; j < 4; j++)
      initial_password[j] = EEPROM.read(j);
    if (!(strncmp(password, initial_password, 4))) {
      oled.clear();
      oled.println("Kod poprawny!");
      digitalWrite(GRN, HIGH);
      beepOnce();
      delay(2000);
      digitalWrite(GRN, LOW);
      oled.clear();
      oled.println("Podaj kod: ");
      oled.set2X();
      oled.setCursor(13, 3);
      oled.print("____");
      oled.set1X();
      i = 0;
    } else {
      oled.clear();
      oled.println("Zle haslo");
      digitalWrite(RED, HIGH);
      beepError();
      delay(1000);
      digitalWrite(RED, LOW);
      oled.clear();
      oled.print("Podaj kod: ");
      oled.set2X();
      oled.setCursor(13, 3);
      oled.print("____");
      oled.set1X();
      i = 0;
    }
  }
}

void change() {
  int j = 0;
  oled.clear();
  oled.print("Stary kod: ");
  oled.set2X();
  oled.setCursor(13, 3);
  oled.print("____");
  oled.set1X();
  while (j < 4) {
    char key = keypad_key.getKey();
    if (key) {
      beepKey();
      stroke++;
      oled.setCursor(stroke * 13, 3);
      oled.set2X();
      new_password[j++] = key;
      oled.print(key);
      oled.set1X();
    }
    key = 0;
  }
  delay(500);
  if ((strncmp(new_password, initial_password, 4))) {
    oled.clear();
    oled.print("Zly kod");
    beepError();
    oled.print("Podaj ponownie: ");
    oled.set2X();
    oled.setCursor(13, 3);
    oled.print("____");
    oled.set1X();
    delay(1000);
  } else {
    j = 0;
    stroke = 0;
    oled.clear();
    oled.print("Nowy kod: ");
    oled.set2X();
    oled.setCursor(13, 3);
    oled.print("____");
    oled.set1X();
    while (j < 4) {
      char key = keypad_key.getKey();
      if (key) {
        beepKey();
        initial_password[j] = key;
        stroke++;
        oled.setCursor(stroke * 13, 4);
        oled.set2X();
        oled.print(key);
        oled.set1X();
        EEPROM.write(j, key);
        j++;
      }
    }
    oled.setCursor(0, 2);
    oled.print("Kod zmieniony!");
    delay(2000);
    stroke = 0;
  }
  oled.clear();
  oled.print("Podaj kod: ");
  oled.set2X();
  oled.setCursor(13, 3);
  oled.print("____");
  oled.set1X();
  key_pressed = 0;
}

void initialpassword() {
  for (int j = 0; j < 4; j++)
    EEPROM.write(j, j + 49);
  for (int j = 0; j < 4; j++)
    initial_password[j] = EEPROM.read(j);
}

void beepKey() {
  digitalWrite(BUZ, HIGH);
  delay(50);
  digitalWrite(BUZ, LOW);
}

void beepOnce() {
  digitalWrite(BUZ, HIGH);
  delay(500);
  digitalWrite(BUZ, LOW);
}

void beepError() {
  digitalWrite(BUZ, HIGH);
  delay(200);
  digitalWrite(BUZ, LOW);
  delay(200);
  digitalWrite(BUZ, HIGH);
  delay(200);
  digitalWrite(BUZ, LOW);
  delay(200);
  digitalWrite(BUZ, HIGH);
  delay(200);
  digitalWrite(BUZ, LOW);
}
