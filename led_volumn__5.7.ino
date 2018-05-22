#include <DHT.h>
#include <Wire.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
#define PIN               6
#define MAX9744_I2CADDR   0x4B
#define SoundSensorPin A1  //this pin read the analog voltage from the sound level meter
#define VREF  5.0  //voltage on AREF pin,default:operating voltage


#ifdef __AVR__
  #include <avr/power.h>
#endif

DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(120, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_TCS34725 tcs   = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

//constant set for sound detection
const int sampleWindow = 2000; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
int     in        = 255;    //keep the time
int     h         = 8;      //defult time 8am
int     memory    = 255;
int     intensity = 0;
int8_t  thevol    = 0;
int8_t  vol       = 10;    //get volume
int     vol_mem   = 0;    //memory volume 
float   C = 0; //correction parameter 
float   vol_C = 0;

void setup(void) {
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  strip.begin();

  if (tcs.begin()) Serial.println("Found sensor");
  else { Serial.println("No TCS34725 found ... check your connections"); while (1);}

  strip.show(); // Initialize all pixels to 'off'
  while(Serial.read()>= 0){}//clear serialport
  if (! setvolume(thevol)) { Serial.println("Failed to set volume, MAX9744 not found!"); while (1);}
}

boolean setvolume(int8_t v) {
  if (v > 63) v = 63;      // can't be higher than 63 or lower than 0
  if (v < 0)  v =  0;
  Wire.beginTransmission(MAX9744_I2CADDR);
  Wire.write(v);
  if (Wire.endTransmission() == 0) return true;
  else return false;
}

void sensors (void) {
  //sound detect and control 
  float voltageValue,dbValue,sumValue = 0,aveValue;
  for(int k = 0; k < 100; k++){
  voltageValue = analogRead(SoundSensorPin) / 1024.0 * VREF;
  dbValue = voltageValue * 50.0;  //convert voltage to decibel value
  sumValue = sumValue + dbValue;
  }
  aveValue = sumValue / 100;
  vol_C = dbValue - C;              //correction 
  //vol = (vol_C / 4.5 + 8) * 1.5;   //give vol
  vol = (vol_C / 4 + 6) * 1.5;
  //C = 1.5984 * vol + 19.108 - dbValue ;   //refresh C
  C = 1.5984 * vol + 40 - dbValue ;
  //Serial.print ("vol:");Serial.print (vol);Serial.println (" ");
  /*Serial.print ("sound:");*/Serial.print (aveValue);Serial.print(" ");//Serial.println ("dBA");
  //if(vol != vol_mem) volume_gradual_change(); //gurdually change the masking volume 
  
  //light detection
  uint16_t r, g, b, c, lux;
  tcs.getRawData(&r, &g, &b, &c);
  lux = tcs.calculateLux(r, g, b);

  // Display the results (light is measured in lux)
  //Serial.print("Lux: "); 
  Serial.print(lux, DEC); Serial.print(" ");
  //Serial.print("Green: "); 
  Serial.print(g, DEC); Serial.print(" ");
  //Serial.print("Blue : "); 
  Serial.print(b, DEC); Serial.print(" ");
 
   // Get a new temp sensor event and Display the temprature results 
  sensors_event_t event2;  
  dht.temperature().getEvent(&event2);
  dht.humidity().getEvent(&event2);
  if (isnan(event2.temperature)) Serial.println("Error reading temperature!");
  else {
    float T = event2.temperature;
    //Serial.print("Temperature: "); Serial.print(T); Serial.print("°C ");
    T = 32 + T * 1.8;
    Serial.print(T); Serial.print(" ");//Serial.println("°F ");
  }
  
  // Get a new humidity sensor event and Display the humidity results 
  if (isnan(event2.relative_humidity)) Serial.println("Error reading humidity!");
  else { //Serial.print("Humidity: "); 
    Serial.print(event2.relative_humidity); 
    //Serial.println("%"); }
  intensity = lux;
  Serial.println(" ");
  }
}

void gettime(void){     // read time data from serial port
  if(Serial.available()>0){ delay(100); h =Serial.parseInt(); }
  while(Serial.read() >= 0){} // clear serial buffer
}

void led(){   //led control
  gettime();
  if (h >= 8  and h < 20)  {colorWipe(strip.Color( 0,  0, 0), 0); in = 255;}  //8:00-20:00 led light off
  if (h >= 0  and h <  6)  {colorWipe(strip.Color(64, 10, 0), 0); in = 255;}  //0:00-5:59 dim led light
  if (h >= 6  and h <  8)  ledautoup();   //6:00-7:59 led light auto up
  if (h >= 20 and h < 24)  { ledautodown();} //after20:00, when room lights off, led on and gradually goes down
}

uint32_t orange = strip.Color(64, 10, 0);
void ledautodown(){
    if (in < 15 ) {colorWipe(orange, 50);} //keep dim 
      else {
        for(in = memory;in >= 10; in--){    //intensity decrease from 255 to 10
          float led_intense = in / 64;
          colorWipe(strip.Color(led_intense * 64, led_intense * 10, 0), 0);
          sensors();
          if(intensity > 1000){ memory = in; break; }  //stop decreasing when sudden room lights on
          gettime();
          if (h >= 8  and h < 20)  {colorWipe(strip.Color( 0,  0, 0), 0); in = 255;break;}  //8:00-20:00 led light off
          if (h >= 0  and h <  6)  {colorWipe(strip.Color(64, 10, 0), 0); in = 255;break;}  //0:00-5:59 dim led light
         }
      }
}

void ledautoup(){
        for(int i = 10;i <= 255; i++){    //intensity increase from 10 to 255
          float led_intense = i / 64;
          colorWipe(strip.Color(led_intense * 64, led_intense * 10, 0), 0);
          sensors();
          if(intensity > 1000){ break; }  //stop when enough light in the room 
        }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void volume(int8_t volume){
  if (volume > 63) volume = 63;
  if (volume < 0 ) volume = 0 ;
  setvolume(volume);
}

void volume_gradual_change(){
  if(vol > vol_mem){for(int8_t i = vol_mem; i <= vol; i++){ volume(i); delay(20);}}
  if(vol < vol_mem){for(int8_t i = vol_mem; i >= vol; i--){ volume(i); delay(20);}}
  vol_mem = vol;
}

void loop(void){
  sensors();
  led();
}

