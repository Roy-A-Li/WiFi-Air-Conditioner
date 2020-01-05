#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <WiFiNINA.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);

const int relaisPin = 7;
const int buttonOne = 10;
const int buttonTwo = 9;
const int buttonThree = 8;
const long interval = 1000;
boolean firstTime = true;
boolean passPage = false;
boolean passFound = false;
int numSsid = 0;
int selectedNetwork = 0;
int tempCounter;
int desiredTemp;
int toggleRelay;
double temperature;
char ssid[32]; // Network SSID
char pass[128]; // Network password
char apSsid[] = "ACWiFiSetup";  // Network SSID
char apPass[] = "123456789"; // Network password
int status = WL_IDLE_STATUS;  // WiFi status
WiFiServer server(80);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  // Starting values
  desiredTemp = 65;
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

  lcd.setCursor(0, 0); // Set the cursor to column 1, line 1
  lcd.print("Connecting..."); // Idle while connecting to WiFi

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

  if (firstTime == false) {
    for (int i = 0; i < 10; i++) {
      Serial.print("Connecting to: ");
      Serial.print(ssid);
      Serial.print("\t");
      Serial.print("Password: ");
      Serial.print(pass);
      Serial.print("\t");
      Serial.print("Status: ");
      Serial.print(WiFi.status());
      Serial.println();
      status = WiFi.begin(ssid, pass);
      delay(10000);
      if (status == WL_CONNECTED) {
        i = 10;
      }
    }
  }

  if (status != WL_CONNECTED) {
    Serial.println("Please set up a network");
    listNetworks();
    status = WiFi.beginAP(apSsid, apPass);
    delay(5000);

    lcd.clear();
    lcd.setCursor(0, 0); // Set the cursor to column 1, line 1
    lcd.print("Please set up a"); // Idle while connecting to WiFi
    lcd.setCursor(0, 1); // Set the cursor to column 1, line 1
    lcd.print("WiFi network"); // Idle while connecting to WiFi
    delay(5000);

    lcd.clear();
    lcd.setCursor(0, 0); // Set the cursor to column 1, line 1
    lcd.print("WiFi setup pass:"); // Idle while connecting to WiFi
    lcd.setCursor(0, 1); // Set the cursor to column 1, line 1
    lcd.print("123456789"); // Idle while connecting to WiFi
    delay(5000);

    server.begin();
    wifiInfo();
  } else {
    Serial.print("You're connected to the network");

    lcd.clear();
    lcd.setCursor(0, 0); // Set the cursor to column 1, line 1
    lcd.print("WiFi connection"); // Idle while connecting to WiFi
    lcd.setCursor(0, 1); // Set the cursor to column 1, line 1
    lcd.print("successful!"); // Idle while connecting to WiFi
    delay(3000);

    server.begin();
    wifiInfo();
  }
}

void loop()
{
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    Serial.println("NEW CLIENT");
    String currentLine = ""; // Stores incoming data from client
    while (client.connected()) {
      if (client.available()) {
        char c = client.read(); // Reads next byte from server
        Serial.write(c);
        if (c == '\n') {
          // If end of HTTP request
          if (currentLine.length() == 0) {
            // HTTP header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            if (status == WL_AP_LISTENING) {
              if (passPage == false) {
                for (int curNetwork = 0; curNetwork < numSsid; curNetwork++) {
                  client.print(WiFi.SSID(curNetwork));
                  client.print(" ");
                  client.print("<a href=\"");
                  client.print("/");
                  client.print(curNetwork);
                  client.print("network");
                  client.print("\">CONNECT</a><br>");
                }
              } else {
                client.print("<form>");
                client.print("<label for=\"pass\">WiFi Password: </label>");
                client.print("<input type=\"password\" id=\"pass\" name=\"password\" size =\"50\" method=\"post\">");
                client.print("<p>");
                client.print("<input type=\"Submit\" Value=\"Submit\">");
                client.print("</form>");
              }
              client.println();
              break;
            } else {
              // Content of HTTP response
              client.print("Click <a href=\"/relayOn\">here</a> turn the relay on<br>");
              client.print("Click <a href=\"/relayOff\">here</a> turn the relay off<br>");
              client.print("Click <a href=\"/tempInc\">here</a> increase temperature<br>");
              client.print("Click <a href=\"/tempDec\">here</a> decrease temperature<br>");
              client.println();
              break;
            }
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { // If client client gives request, add to currentLine
          currentLine += c;
        }
        if (status == WL_AP_LISTENING) {
          if (passPage == false) {
            for (int curNetwork = 0; curNetwork < numSsid; curNetwork++) {
              String httpGet = "GET /";
              httpGet.concat(curNetwork);
              httpGet.concat("network");
              if (currentLine.endsWith(httpGet)) {
                selectedNetwork = curNetwork;
                passPage = true;
              }
            }
          } else {
            if (currentLine.indexOf("?password=") > 0 && currentLine.endsWith(" HTTP/1.1") && passFound == false) {
              int passStartIndex = currentLine.indexOf("?password=") + 10;
              int passEndIndex = currentLine.indexOf(" HTTP/1.1");
              for (int thisChar = passStartIndex; thisChar < passEndIndex; thisChar++) {
                pass[thisChar - passStartIndex] = currentLine.charAt(thisChar);
              }
              passFound = true;
            }
            if (passFound == true) {
              String networkName = WiFi.SSID(selectedNetwork);
              networkName.toCharArray(ssid, networkName.length() + 1);
              Serial.println(networkName);
              firstTime = false;
              passPage = false;
              passFound = false;
              client.stop();
              Serial.println("client disconnected");
              WiFi.end();
              setup();
            }
          }
        } else {
          // Check client request
          if (currentLine.endsWith("GET /relayOn")) {
            toggleRelay = 1;
          }
          if (currentLine.endsWith("GET /relayOff")) {
            toggleRelay = 0;
          }
          if (currentLine.endsWith("GET /tempInc")) {
            desiredTemp++;
          }
          if (currentLine.endsWith("GET /tempDec")) {
            desiredTemp--;
          }
        }
      }
    }
    // Close connection
    client.stop();
    Serial.println("client disconnected");
  }

  // Requests temperature every 660ms
  if (tempCounter == 3) {
    sensors.requestTemperatures();
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

  lcd.setCursor(0, 0); // Set the cursor to column 1, line 1
  lcd.print("Current: ");
  lcd.print(temperature);
  lcd.print((char)0);
  lcd.print("F");

  lcd.setCursor(0, 1); // Set the cursor to column 1, line 2
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

void wifiInfo() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Board local ip
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void listNetworks() {
  // scan for nearby networks:
  numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }
  // print the network number and name for each network found:
  for (int currentNet = 0; currentNet < numSsid; currentNet++) {
    Serial.print(currentNet + 1);
    Serial.print(") ");
    Serial.print("\tSSID: ");
    Serial.println(WiFi.SSID(currentNet));
  }
  Serial.println();
}
