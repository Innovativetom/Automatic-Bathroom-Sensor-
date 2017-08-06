/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "cb352722349b40af96cb9753e78fd37d";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "THE WIFI";
char pass[] = "demiandthomas";

// Setting up the pin and DHT version
#define DHTTYPE DHT11   // DHT Shield uses DHT 11
#define DHTPIN D4       // DHT Shield uses pin D4
DHT dht(DHTPIN, DHTTYPE);

// Set Colors of LEDs
/* Not currently used
//Widget Colors
#define BLYNK_Green     "#23C48E"
#define BLYNK_Blue      "#04C0F8"
#define BLYNK_Red       "#D3435C"
*/
//Set up of the Humiditly LED & override LED 
//these are software LEDs in app
WidgetLED humLED(V1); 
WidgetLED overrideLED(V2);

//this timer is used for the blynk app/server and to call data from the sensor
BlynkTimer timer;

//Sensor varables
float h = 0;
float t = 0;

//Fan varables
int fanState = 0;
bool fanOverride = false;

// countdown variables and timer
int CountdownRemain;
int CountdownTimer;

SimpleTimer overrideShutOffTimer;

// this funtion gets the data from the sensro and send it to the blynk server for you to see on the app
void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature(true); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  int hAsInt = int(h); // converts to int removing unessisary decimal points
  int tAsInt = int(t);
  Blynk.virtualWrite(V5, hAsInt);
  Blynk.virtualWrite(V6, tAsInt);
  fanControl();
}
// Fan Override Button
BLYNK_WRITE(V0)
{
  int buttonState = param.asInt();
Serial.print("button pressed: ");
Serial.println(buttonState);

  if(buttonState == 1){
    fanState = 1;
    fanOverride = true;
    overrideLED.on();  
    CountdownRemain = 600;
    overrideShutOffTimer.enable(CountdownTimer);
//Debug Calls
Serial.print("button State: ");
Serial.print(buttonState);
Serial.print(" fan State: ");
Serial.print(fanState);
Serial.print(" fan override: ");
Serial.println(fanOverride);
  }else{
    fanState = 0;
    fanOverride = true;
    CountdownRemain = 600;
    overrideShutOffTimer.enable(CountdownTimer);
    overrideLED.on(); 
 //Debug Calls
Serial.print("button State: ");
Serial.print(buttonState);
Serial.print(" fan State: ");
Serial.print(fanState);
Serial.print(" fan override: ");
Serial.println(fanOverride);
  }
    fanControl();
}

void fanControl()
{
    if(h > 60 && fanOverride != true){
      fanState = 1; //ON
      Blynk.virtualWrite(V0, fanState);
      humLED.on();  
Serial.print("over 60H, ");
    }
    else if(h < 50 && fanOverride != true){
      fanState = 0; //OFF
      Blynk.virtualWrite(V0, fanState);
      humLED.off();  
Serial.print("less than 50H, ");   
    }
    else{
Serial.print("fan override, ");
    }
    digitalWrite(D1, fanState);

Serial.print(" fan State: ");
Serial.print(fanState);
Serial.print(" fan override: ");
Serial.println(fanOverride);
}
/*void overrideShutOff(){
  fanOverride = false;
  manLED.off(); 
  Serial.print("override shut off");
}*/

void CountdownTimerFunction() {
Serial.print("countdown function called ");
  CountdownRemain--; // remove 1 every second
Serial.println(CountdownRemain);
  Blynk.virtualWrite(V3, CountdownRemain);
  if (!CountdownRemain) { // check if CountdownRemain == 0/FALSE/LOW
    overrideShutOffTimer.disable(CountdownTimer); // if 0 stop timer
    CountdownRemain = 600;
    fanOverride = false;
    overrideLED.off();
Serial.println("counter if");
  } else {
    //manLED.off();
Serial.println("counter else");
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(D1, OUTPUT); // Set the relay output up
  Blynk.begin(auth, ssid, pass);
   // Setup a function to be called every 5 seconds
  timer.setInterval(5000L, sendSensor);
  //Shut off the manual over ride
  CountdownTimer = overrideShutOffTimer.setInterval(1000L, CountdownTimerFunction); 
  overrideShutOffTimer.disable(CountdownTimer);
}

void loop()
{
  Blynk.run();
  timer.run();
  overrideShutOffTimer.run();
}

