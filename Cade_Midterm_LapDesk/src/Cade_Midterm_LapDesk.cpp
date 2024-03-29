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
#include "IoTTimer.h"

SYSTEM_MODE(MANUAL);
//DECLARE


//Declare the timer and Pixelfill function
IoTTimer timerOn;
void pixelFill(int first, int last, int color);

//Declare my neopixel strip
Adafruit_NeoPixel pixels (30, SPI1, WS2812B);
const int LEDs = 30;
int pixelBright;
int i;
int z;

//declare my wemo
const int myWemo=5;

//Hue Bulb
const int BULB=2; 
int color;

Encoder myEncoder(D18,D19);
//  Hue variables
int brightness; // need a variable for mapping
int position;
int newPosition;


//declare my (encoder) button
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

//declare my fourth button
int buttonFour = D14;
Button buttonCuatro (buttonFour);
bool buttonCuatroState;
int switchCuatroState;

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
    display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);

    display.clearDisplay();


// //initialize the pixels and timer function
pixels.begin();
pixels.setBrightness(8);
pixels.clear();
pixels.show();

 timerOn.startTimer(5000);

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
display.printf ("The temperature is %0.1f %c\n",tempF,degree);
display.printf("The humidity is %0.1f\n", humidRH);
display.printf("The pressure is %0.1f\n", mercury);
display.display();

//print to monitor
Serial.printf ("The temperature is %0.1f %c\n",tempF,degree);
Serial.printf("The humidity is %f\n", humidRH);
Serial.printf("The pressure is %f\n", mercury); 

//second button changes colors
if (buttonDos.isClicked()) {
    (color++);
}
  Serial.printf("Setting color of bulb %i to color %06i\n",BULB,HueRainbow[color%7]);
  
//ENCODER BUTTON DATA
// Encoder switch turns lights on and off, knob increase brightness.exit

buttonPinState = digitalRead(buttonpin);

//if button is clicked, turn on/off and switch states. WORKS
if (button.isClicked()) {
  switchState = !switchState;
  Serial.println("Encoder button is clicked");
}
  
   //ENCODER KNOB
position = myEncoder.read();
  //if (position != newPosition){
        Serial.printf("Encoder is %d\n", position);
      //  newPosition = position; 
  //}
  
 // bound the inputs
  if (position> 95) {
    myEncoder.write(95);
    position = 95;
  }

  if (position <0) {
    myEncoder.write(0);
    position = 0;
  }

  //map one encoder readouts to the HUE LIGHT 
 // newPosition = map(position,0,95,0,255);

//map the encoder to the brightness of the bulb
  brightness = map(position,0,95,0,255); 

  setHue(BULB,switchState,HueRainbow[color%7],brightness,255);


//BUTTON TO CONTROL WEMO   
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

// //TIMER CONTROL
// //timer controls, 5 seconds to active
if (timerOn.isTimerReady()) {
  pixelFill(0,29,violet);
  pixels.setBrightness(8);
}


// //BUTTON FOUR RESETS THE TIMER
if (buttonCuatro.isClicked()) {
pixels.clear();
pixels.show();
  timerOn.startTimer(5000);
}



}





//FUNCTIONS
// FUNCTIONS
void  pixelFill (int first, int last, int color) {
   int i;
   for (i = first; i<= last; i++) {
      pixels.setPixelColor(i,color);
      pixels.show();

   }

}