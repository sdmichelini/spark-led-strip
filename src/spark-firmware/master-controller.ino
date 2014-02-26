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


//Command Variables
String _command="raffff00";
String _lastNotifiedCommand="raffff00";
//Loop Count and Notification Variables
unsigned long _loopCount=0;
unsigned long _lastLoop=0;

//Color Constants
const String NIGHT_COLOR ="rcFF7800";
const String DIM_NIGHT_COLOR = "rc0F0A00";
const String DAY_COLOR ="rcFFE9C8";
const String OFF_COLOR ="rc000000";
const String NOTIFICATION_COLOR ="rbffffff";


void setup() {
    Spark.function("setV",setValue);
    Wire.begin();                
    Serial.begin(9600);
}

bool inRange(uint8_t val, uint8_t low, uint8_t high){
    return ((val<=high)&&(low<=val));
}

void loop(){
    if(_command=="notify"){
       if(_loopCount>(_lastLoop+1)){
           _command=_lastNotifiedCommand;
           setValue(_command);
       }
    }
   
    Serial.println("Current Color Command: "+_command);
    _loopCount++;
    delay(1000);
}
//Wrapper for i2c functionality
bool i2c_write(String send){
    Wire.beginTransmission(4); 
    Wire.write(send.c_str());         
    Wire.endTransmission(true);    
}

//Handle Automatic Functions
//Returns true if a valid auto command is recognized
bool handleAutoCommand(String Command){
    if(_command=="auto")
    {
        if(Command=="sunrise"){
            i2c_write(DAY_COLOR);
            Serial.println("Auto: Sunrise");
            return true;
        }
        else if(Command=="sunset"){
            i2c_write(NIGHT_COLOR);
            Serial.println("Auto: Sunset");
            return true;
        }
        else if(Command=="off"){
            i2c_write(OFF_COLOR);
            Serial.println("Auto: Off");
            return true;
        }
        else if(Command=="dim"){
            i2c_write(DIM_NIGHT_COLOR);
            Serial.println("Auto: Dim Nigh Color");
            return true;
        }
        return false;
    }
    return false;
}

//Let Spark Decide When to Set LED's when command is auto
int setValue(String Command){
    Serial.println("Recieved Command: "+Command);
    if(handleAutoCommand(Command))return 0;
    //Wait til after to reset command because the command could be an auto command if we are in the auto state
    _command=Command;
    
    if(_command=="auto")
    {
        i2c_write(DAY_COLOR);
        _lastNotifiedCommand="auto";
    }
    //Set it to Auto but Start in Off Position
    else if(_command=="auto%20off")
    {
        i2c_write(OFF_COLOR);
        Serial.println("Auto Off");
        _lastNotifiedCommand="auto";
        _command="auto";
        return 0;
    }
    //Python Detected Sunsets
    else if(_command=="autos")
    {
        i2c_write(NIGHT_COLOR);
        Serial.println("Auto Off");
        _lastNotifiedCommand="auto";
        _command="auto";
        return 0;
    }
    else if(_command=="notify")
    {
        String send=NOTIFICATION_COLOR;
        i2c_write(send); 
        _lastLoop=_loopCount;
    }
    else
    {
        i2c_write(_command);
        _lastNotifiedCommand=_command;
    }
    return 0;
}
