/* Arduino NeoPixel LED Controller by sdmichelini
  
   I2C Slave on address 4. Recieves 8 character strings and parse them to recieve a two character command and a six character string which is converted to a 24-bit hex color value
   Built on NeoPixel demo and converted to use an iterative code style. Look at the rainbow function for an example of hoe it works

   Copyright (c) 2014 sdmichelini

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   TODO: Clean up code
   Add More command

*/
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

int PIN =5;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(59, PIN, NEO_GRB + NEO_KHZ800);
//The Requested RGB Values and the Current Ones Used for Fading and Modified by updateValues()
int _requested_r,_current_r;
int _requested_g,_current_g;
int _requested_b,_current_b;

void setup()
{
  Wire.begin(4);        // join i2c bus at address 4
  Wire.onReceive(requesti2c); //register event to recieve i2c commands
  Serial.begin(9600);  // start serial for output
  strip.begin(); 
  strip.setBrightness(255);
  strip.show();
  //Starts Off Wanting to Go To Red
  _requested_r=255;
  _requested_g=0;
  _requested_b=0;
  //Starts off dim
  _current_r=0;
  _current_g=0;
  _current_b=0;
  pinMode(4,INPUT); 
}

String _command="";
String _hexString="0xff0000";
String _lastHexString="";

const int RECIEVE_SIZE=8;
const int COMMAND_SIZE=2;

void requesti2c(int howMany){
  Serial.println("reading i2c");
  //Only 8-Character Strings are Accepted
  if(howMany!=RECIEVE_SIZE){
     Serial.println("Improper Byte Count Recieved of: "+String(howMany));
     return; 
  }
  String recieve="";
  while(Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    recieve+=c;         // print the character
  }
  //Parse Command
  _command=recieve.substring(0,COMMAND_SIZE);
  //Generate Hex String
  _hexString="0x"+recieve.substring(COMMAND_SIZE);
  Serial.println(recieve);
  //Convert to 24-bit hex
  uint32_t hexValue=(strtoul(_hexString.c_str(),NULL,0));
  //We are requesting to go to these values. They will fade to these values from the current oens
  _requested_r=((hexValue >> 16) & 0xFF);
  _requested_g=((hexValue >> 8) & 0xFF);
  _requested_b=((hexValue) & 0xFF);
  
}

int _loopCount=0;

const int MAX_COLOR_CHANGE=2;

void updateValues(){
   int deltaR=_requested_r-_current_r; 
   int deltaG=_requested_g-_current_g; 
   int deltaB=_requested_b-_current_b; 
   if(deltaR>MAX_COLOR_CHANGE)deltaR=MAX_COLOR_CHANGE;
   else if(deltaR<-MAX_COLOR_CHANGE)deltaR=-MAX_COLOR_CHANGE;
   if(deltaG>MAX_COLOR_CHANGE)deltaG=MAX_COLOR_CHANGE;
   else if(deltaG<-MAX_COLOR_CHANGE)deltaG=-MAX_COLOR_CHANGE;
   if(deltaB>MAX_COLOR_CHANGE)deltaB=MAX_COLOR_CHANGE;
   else if(deltaB<-MAX_COLOR_CHANGE)deltaB=-MAX_COLOR_CHANGE;
   _current_r+=deltaR;
   _current_g+=deltaG;
   _current_b+=deltaB;
}

void loop()
{
    _loopCount++;
    if(_loopCount>10000)_loopCount=0;

    if(_command=="rb")//Rainbow
      rainbow();
    else if(_command=="ra")//Fade in and out
      fadeColor(strtoul(_hexString.c_str(),NULL,0));
    else{
      updateValues();//Only do fade if strip is solid
      colorWipe(strip.Color(_current_r,_current_g,_current_b));
    }
    
    delay(10);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      
  }
  strip.show();
}

void colorSetWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      
  }
  delay(wait);
}

void fadeColor(uint32_t c)
{
   int loopCount=_loopCount%512;
    if(loopCount>255)loopCount=255-(loopCount-256);
   float br_mult=((float)loopCount/255.0);
  
   
   colorWipe(strip.Color((float)((c >> 16) & 0xFF)*br_mult,(float)((c >> 8) & 0xFF)*br_mult,(float)((c) & 0xFF)*br_mult));
     
}

void rainbow() {
  uint16_t i, j;
  j=_loopCount%256;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i+j) & 255));
  }
  strip.show();
}


uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

