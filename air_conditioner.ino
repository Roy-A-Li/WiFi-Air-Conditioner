#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);

const int relaisPin = 7;
const int buttonOne = 10;
const int buttonTwo = 9;
const int buttonThree = 8;
const long interval = 1000;

int tempCounter;
int desiredTemp;
int toggleRelay;

double temperature;

DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  // Starting values
  desiredTemp = 70;
  tempCounter = 0;
  toggleRelay = 0;

  // Relay and push buttons
  pinMode(relaisPin, OUTPUT);
  pinMode(buttonOne, INPUT);
  pinMode(buttonTwo, INPUT);
  pinMode(buttonThree, INPUT);

  Serial.begin(9600);

  sensors.begin();

  // Get current temperature in Fahrenheit
  sensors.requestTemperatures();
  temperature = sensors.getTempFByIndex(0);

  lcd.init();  // Initialize the lcd
  lcd.backlight();  // Open the backlight

  lcd.setCursor(15, 1); // Set the cursor to column 15, line 2
  lcd.print("o"); // Begins in an off state

  // Degree symbol configuration
  byte degreeSymbol[8] = {
    0b00110,
    0b01001,
    0b01001,
    0b00110,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcd.createChar(0, degreeSymbol);
}

void loop()
{
  // Requests temperature every 660ms
  if (tempCounter == 3) {
    Serial.println("Requesting temperatures...");
    sensors.requestTemperatures();
    Serial.println("DONE");

    Serial.print("Temperature is: ");
    Serial.println(sensors.getTempFByIndex(0));
    temperature = sensors.getTempFByIndex(0);

    tempCounter = 0;
  }

  // Increases temperature when buttonThree is pressed
  if (digitalRead(buttonTwo) == LOW && digitalRead(buttonThree) == HIGH 
      && desiredTemp <= 120) {
    desiredTemp++;
  }

  // Decreases temperature when buttonTwo is pressed
  if (digitalRead(buttonThree) == LOW && digitalRead(buttonTwo) == HIGH
      && desiredTemp >= -120) {
    desiredTemp--;
  }

  lcd.setCursor(0, 0); // Set the cursor to column 0, line 1
  lcd.print("Current: ");
  lcd.print(temperature);
  lcd.print((char)0);
  lcd.print("F");

  lcd.setCursor(0, 1); // Set the cursor to column 0, line 2
  lcd.print("Desired: ");
  lcd.print(desiredTemp);
  lcd.print((char)0);
  lcd.print("F");

  // Toggles relay module and displays on/off state
  if (digitalRead(buttonOne) == LOW && toggleRelay == 0) {
    toggleRelay = 1;
    lcd.setCursor(15, 1); // Set the cursor to column 15, line 2
    lcd.print("|");
  } else if (digitalRead(buttonOne) == LOW && toggleRelay == 1) {
    toggleRelay = 0;
    lcd.setCursor(15, 1); // set the cursor to column 15, line 2
    lcd.print("o");
  }

  // Powers fans only when desired temperature is less than current
  if (toggleRelay == 1) {
    if (temperature > desiredTemp) {
      digitalWrite(relaisPin, HIGH);
    } else {
      digitalWrite(relaisPin, LOW);
    }
  } else {
    digitalWrite(relaisPin, LOW);
  }

  tempCounter++;
  delay(220);
}
