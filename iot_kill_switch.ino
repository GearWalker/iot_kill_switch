/*
 *  This sketch sends message via HTTP POST requests to LINE NOTIFY API service.
 *
 *  You need to get LINE developer ID and API TOKEN from https://notify-bot.line.me/my/ 
 *  and paste them below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>

// constants won't change. They're used here to set pin numbers:
const int PIN_BTN = 0;  // the number of the pushbutton pin
const int PIN_LED = 2;     // the number of the LED pin
const char* LINE_API_HOST = "notify-api.line.me";

// please change this below to match your needs.
const char* WIFI_SSID     = "your-wifi-ssid";
const char* WIFI_PASS    = "your-wifi-password";
const char* LINE_API_TOKEN = "your-line-notify-token";

// Variables will change:
int btnState = HIGH;        // the current reading from the input pin
int btnLastState = LOW;  // the previous reading from the input pin

void setup()
{
  Serial.begin(115200);
  
  // set I/O pins mode
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_LED, OUTPUT);
  delay(500);

  // turn LED ON
  digitalWrite(PIN_LED, HIGH);

  Serial.println("\r\nSenses IoT kill switch demo!");
  Serial.print("Connecting WiFi ["+ String(WIFI_SSID) +"] ");

  // connect to wifi-ap
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // turn LED OFF
  digitalWrite(PIN_LED, LOW);
}

void loop()
{
  // a simple read the state of the switch (not efficient yet):
  btnLastState = btnState;
  btnState = digitalRead(PIN_BTN);

  // Detect If the switch just pressed.
  if((btnState != btnLastState)&&(btnState == LOW))
  {
    // turn LED ON
    digitalWrite(PIN_LED, HIGH);
  
    //Check WiFi connection status
    if(WiFi.status() == WL_CONNECTED) 
    {
      WiFiClientSecure httpsclient;
      if(!httpsclient.connect(LINE_API_HOST,443))
      {
        Serial.println("Connection failed.");
      }
      else
      {
        Serial.println("Connecting...");
        
        String body = "message=Hello from IoT Button!";
        String postRequest = "";
        postRequest += "POST  /api/notify HTTP/1.1\r\n";
        postRequest += "Host: " + String(LINE_API_HOST) + "\r\n";
        postRequest += "Authorization: Bearer " + String(LINE_API_TOKEN) + "\r\n";
        postRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
        postRequest += "Content-Length: " + String(body.length()) + "\r\n";
        postRequest += "\r\n" + body;

        httpsclient.print(postRequest);
        while(httpsclient.connected())
        {
          String line = httpsclient.readStringUntil('\n');
          Serial.println(line);
          if(line == "\r")
          {
            break;
          }
        }
      }
    }
    // delay to prevent flooding
    delay(1000);
    Serial.println("Done!");
    // turn LED OFF
    digitalWrite(PIN_LED, LOW);
  }
}