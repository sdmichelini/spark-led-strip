/*Spark Master Controler by sdmichelini
  
   Heart of the lighting setup. Has control over the Arduino and many high levels commands are handled here. Also master of the i2c bus

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
   Add More Documentation
   Add More Commands

*/


// This #include statement was automatically added by the Spark IDE.
//Spark Time library by Brian Ogilvie           
//Can be found at https://github.com/bkobkobko/SparkCoreLibraries
#include "SparkTime.h"

//We need time for when we enter auto mode
UDP UDPClient;
SparkTime rtc;

unsigned long currentTime;
//Default Command
String _command="raffff00";
String _lastCommand="";
String _lastNotifiedCommand="raffff00";

unsigned long _loopCount=0;
unsigned long _lastLoop=0;


void setup() {
    //Connecting to the Clock
    rtc.begin(&UDPClient, "north-america.pool.ntp.org");
    rtc.setTimeZone(-5); // gmt offset
    Spark.function("setV",setValue);

    //join i2c as master
    Wire.begin();                
    
    Serial.begin(9600);
    pinMode(D2,OUTPUT);
    
}


//simply check if a value is between two numbers
bool inRange(uint8_t val, uint8_t low, uint8_t high){
    return ((val<=high)&&(low<=val));
}




void loop(){
   digitalWrite(D2,HIGH);
   //If we have notified for more than a loop go back to last color
   if(_command=="notify"){
       if(_loopCount>(_lastLoop+1)){
           _command=_lastNotifiedCommand;
           setValue(_command);
       }
   }
   
   Serial.println("Current Color Command:"+_command);
   _loopCount++;
    delay(1000);
}


//Let Spark Decide When to Set LED's when command is auto
int setValue(String Command){
    //If we are currently in auto state
    if(_command=="auto"){
        if(Command=="sunrise"){
            String send = "rcFFE9C8";
            Wire.beginTransmission(4); // transmit to device #4
            Wire.write(send.c_str());         
            Wire.endTransmission(true);    // stop transmitting
            Serial.println("Auto: Sunrise");
            return 0;
        }
            
        else if(Command=="sunset"){
            String send = "rcFF7800";
            Wire.beginTransmission(4); // transmit to device #4
            Wire.write(send.c_str());         
            Wire.endTransmission(true);    // stop transmitting
            Serial.println("Auto: Sunset");
            return 0;
        }
        else if(Command=="off"){
            String send = "rc000000";
            Wire.beginTransmission(4); // transmit to device #4
            Wire.write(send.c_str());         
            Wire.endTransmission(true);    // stop transmitting
            Serial.println("Auto: Off");
            return 0;
        }
    }
    _command=Command;
    Serial.println("Recieved Command: "+Command);
    if(_command=="auto")
    {
        currentTime=rtc.now();
        //Sunset
        if(inRange(rtc.hour(currentTime),17,24)||inRange(rtc.hour(currentTime),0,7)){
            String send = "rcFF7800";
            Wire.beginTransmission(4); // transmit to device #4
            Wire.write(send.c_str());         
            Wire.endTransmission(true);    // stop transmitting
            Serial.println("Auto: Sunset");
            _lastNotifiedCommand="auto";
        }
        //Sunrise
        else{
            String send = "rcFFE9C8";
            Wire.beginTransmission(4); // transmit to device #4
            Wire.write(send.c_str());          
            Wire.endTransmission(true);    // stop transmitting
            Serial.println("Auto: Sunrise");
            _lastNotifiedCommand="auto";
        }
        _lastNotifiedCommand="auto";
        _lastCommand="";
        
    }
    else if(_command=="notify")
    {
        String send="rbffffff";
        Wire.beginTransmission(4); // transmit to device #4
        Wire.write(send.c_str());        
        Wire.endTransmission(true);    // stop transmitting
        _lastLoop=_loopCount;
    }
    else
    {
        
        Wire.beginTransmission(4); // transmit to device #4
        Wire.write(_command.c_str());         
        Wire.endTransmission(true);    // stop transmitting
        _lastNotifiedCommand=_command;
    }
    return 0;
}

