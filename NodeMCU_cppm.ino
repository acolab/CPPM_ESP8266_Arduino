/* 
==== CPPM Decode function for Arduino, ESP8266/NodeMCU ====
This code read "Radio Control" CPPM signal from a receveir
or from a 'trainer port' on transmitter.
Each pulse feed a table for using it during loop.

During loop, call it with
cppmRead();

Pulse time will be available in the table ppmValue[<chan number>]

Arduino ATMega328 usable interrupt pin
D2 = 2
D3 = 3

NodeMCU labels table for GPIO
 D0 = 16 / USER / WAKE
 D1 = 05
 D2 = 04 
 D3 = 00 / FLASH
 D4 = 02 / TXD1
 D5 = 14 /      / HSCLK
 D6 = 12 /      / HMISO
 D7 = 13 / RXD2 / HMOSI
 D8 = 15 / TXD2 / HCS
 D9 = 03 / RXD0
 D10 = 01 / TXD0
 SD0 = MISO / SDD0
 SD1 = MOSI / SDD1
 SD2 = 09 / SDD2
 SD3 = 10 / SDD3
 CMD = CS / SDCMD
  
*/

int cppmPIN = 3; // Set input pin for CPPM signal

unsigned int channel = 0;
unsigned int pulseLenght;
unsigned int pulsePrevious;
unsigned int pulseRising;

int ppmValue[17]; // Create a table for 16 channels max, ppmValue[0] is the sync pulse

void setup() {
  
  pinMode ( cppmPIN, INPUT ); // Set input for CPPM from real world

// A simple 'Hello world" on serial for demo
  Serial.begin (115200);//( 115200 ); // Init serial port for debug
  Serial.println ("CPPM Reading for ESP8266/NodeMCU/Arduino");
  Serial.print ("Reading CPPM Frame on GPIO : ");
  Serial.println (cppmPIN);
  
}  // end of setup

//////////////////////// Starting of loop
void loop() {

  cppmRead();   // Call for a CPPM frame reading

  
 /*  Demo of cppm values reading
 *  You can also directly call the channel you need
 *  ppmValue[3] will return the PPM lenght for channel 3
 *  Note : ppmValue[0] is the sync pulse, not usable value in common case
 */

  for (channel=1; channel < 17; channel++){   //ppmValue[0] is the sync pulse, we dont want to display it
    String out; // needed only for demo code
    out = "Ch";
    out = out + (channel);
    out = out + ":";
    out = out + ppmValue[channel];
    out = out + "us|";
    Serial.print (out);
  }
Serial.println();

// End of demo code



} //////////////////////// End of loop



void cppmRead(){       // Fonction to interrupt loop during a CPPM frame reading
  delay (5);           // Need for stability
// This delay can be removed if time is enough between 2 cppmRead() instructions during loop.
// Crashes occur if you try to call pulseDetect() again during CPPM Frame Reading
  attachInterrupt(digitalPinToInterrupt(cppmPIN), pulseDetect , RISING);
  delay (40); // time to read full CPPM frame - can be reduced for less than 16 channels decoding
  detachInterrupt(digitalPinToInterrupt(cppmPIN));
}

void pulseDetect(){
  pulsePrevious = pulseRising;
  pulseRising = micros();
  pulseLenght = micros() - pulsePrevious;
  
   if (pulseLenght > 3500){    // Sync pulse of CPPM Frame detection
    channel = 0;               // To store sync pulse value in ppmValue[0] 
   }
   
  ppmValue[channel] = pulseLenght;
  channel = channel+1;
}
