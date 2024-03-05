/* 
 * Project SMART LAP DESK-- MIDTERM
 * Description: A lap desk that can control the room, and has a timer function
 * to encourage breaks
 * Author: Chris Cade
 * Date: March 1st 2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */


#include "Particle.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "IoTClassroom_CNM.h"
#include "hue.h"
#include "Encoder.h"
#include "application.h"
#include "colors.h"
#include "neopixel.h"
#include "Adafruit_BME280.h"
#include "Button.h"

SYSTEM_MODE(MANUAL);
//DECLARE

//declare my wemo
const int myWemo=5;

//Hue Bulb
const int BULB=2; 
int color;

Encoder myEncoder (D3, D4);
//  Hue variables
int brightness; // need a variable for mapping

int position = 0;
int newPosition;


//declare my button
int buttonpin = D11;
Button button(buttonpin);
bool buttonPinState;
int switchState;

//declare my second button
int buttonTwo = D12;
Button buttonDos(buttonTwo);
int buttonDosState;

//declare my third button
int buttonThree = D13;
Button buttonTres(buttonThree);
bool buttonTresState;
int switchTresState;

//Declare the BME
Adafruit_BME280 bme;  // the I2C device that measures

float tempC;
float tempF;
float pascals;
float humidRH;
float mercury; 
const char degree = 0xF8;

int status; 


//declare the OLED
const int OLED_RESET =-1;
Adafruit_SSD1306 display(OLED_RESET);


void setup() {

// Initialize the bme
status = bme.begin(0x76);
  if (status == false) {
    Serial.printf("BME at address 0x%02X failed to start",0x76);
  }

  Serial.begin(9600);
  waitFor(Serial.isConnected,15000);

  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
  
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

//initialize the OLED
display.begin(SSD1306_SWITCHCAPVCC,0x3C);
delay(2000);
    display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);
display.printf("Hello world!\n OLED Screen Active\n");
delay(5000);
    display.clearDisplay();

}

void loop() {

  //use bme to read all values
tempC = bme.readTemperature();
pascals = bme.readPressure();
humidRH = bme.readHumidity();

tempF = map(int(tempC),0,100,32,212);
//mercury = map(pascals,29.92,106500,31.95,108100);
mercury = pascals*0.0003; 

//set display parameters fpor OLED
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);  
display.printf ("My temperature is %0.1f %c\n",tempF,degree);
display.printf("My humidity is %0.1f\n", humidRH);
display.printf("My pressure is %0.1f\n", mercury);
display.display();

//print to monitor
Serial.printf ("My temperature is %0.1f %c\n",tempF,degree);
Serial.printf("My humidity is %f\n", humidRH);
Serial.printf("My pressure is %f\n", mercury); 


if (buttonDos.isClicked()) {

    (color++);
}
  
  Serial.printf("Setting color of bulb %i to color %06i\n",BULB,HueRainbow[color%7]);
  
  
//ENCODER BUTTON DATA
buttonPinState = digitalRead(buttonpin);

//if button is clicked, turn on/off and switch states. WORKS
if (button.isClicked()) {
  switchState = !switchState;
}
//map the encoder to the brightness of the bulb
  brightness = map(newPosition,0,95,0,255); 

  //map one encoder readouts to the HUE LIGHT 
  newPosition = map(position,0,95,0,255);

   //ENCODER KNOB
position = myEncoder.read();
  if (position != newPosition){
        Serial.printf("Encoder is %d\n", position);
        newPosition = position; 
  }
    
 // bound the inputs
  if (position> 95) {
    myEncoder.write(95);
    position = 95;
  }

  if (position <0) {
    myEncoder.write(0);
    position = 0;
  }
  setHue(BULB,switchState,HueRainbow[color%7],brightness,255);

//BUTTON TO CONTROL WEMO
//buttonTresState = digitalRead(buttonThree);

//if button three is clicked, turn on and switch states.
if (buttonTres.isClicked()) {
  buttonTresState = !buttonTresState;
}
   
  
if (buttonTresState)   {
  wemoWrite (5,HIGH);
    Serial.printf("Turning on Wemo# %i\n",myWemo);
}
else {
  Serial.printf("Turning off Wemo# %i\n",myWemo);
    wemoWrite(5,LOW);
}
}
     