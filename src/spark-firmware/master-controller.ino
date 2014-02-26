/*Spark Master Controler by sdmichelini
  
   Heart of the lighting setup. Has control over the Arduino and many high levels commands are handled here. Also master of the i2c bus

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
     ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
     POSSIBILITY OF SUCH DAMAGE.

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

