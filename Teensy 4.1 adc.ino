
#include <SD.h>   //Arduino defaut SD Card library
#include <Wire.h>
#include <Adafruit_BMP280.h>


#define ANALOG_PIN A15  //This is the pin to attach the scan output to
#define TRIGGER_PIN 38  //This is the pin to attach the scan trigger to
#define LED 13         //Teensy built in LED pin


const int chipSelect = BUILTIN_SDCARD; //which pin to use for the Teensy 3.6 built in SD Card
bool SDCardAvailable = true; //Boolean to signal if an SD Card is available for writing
String SDString;  //A String class to use to write data lines to the SD Card
bool Serial_Plotter = true; //set this to true to format output for the Arduino plotter.
Adafruit_BMP280 bmp;

int l=1,k=1;
char myfile[11];
int timecheck = millis();

void setup() {
  Serial.begin(115200); //Start the USB Serial monitor.
  delay(3000); //Wait for it to initialize.   //Use the 3.3V supply as the analog reference
  analogReadResolution(12); //Set the ADC resolution, values can range from 8 - 16 bits
  analogReadAveraging(32);  //Set the number of samples to internally average, values can range from 2 - 32
  pinMode(TRIGGER_PIN, INPUT); // set trigger pin as digital input.
  pinMode(LED, OUTPUT); // LED output pin
  bmp.begin(0x76);

  digitalWrite(LED, HIGH);
  /* SD Card setup */
  Serial.print("Initializing SD card...Im really new");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    SDCardAvailable = false;
  }
  else {
    Serial.println("SD card initialized.");
  }

  digitalWrite(LED, LOW);
  // Serial.println(F("BMP280 test"));
  //unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  // status = bmp.begin(0x76);
  //if (!status) {
  //Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
  //                "try a different address!"));
  //Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
  //Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
  //Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
  //Serial.print("        ID of 0x60 represents a BME 280.\n");
  //Serial.print("        ID of 0x61 represents a BME 680.\n");
  //while (1) delay(10);
  //}

  /* Default settings from datasheet.
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  */

}

void loop() {
  int i = 0;
  int j = 0;
  float temperature = 0;
  float pressure = 0;
  uint32_t samples[576];
  sprintf(myfile, "sky%d.bin", k);

  samples[0] = millis();
  Serial.println(samples[0]);
  for (j = 1; j > 0; j--)
  {
    while (digitalReadFast(TRIGGER_PIN) == 1);
    while (digitalReadFast(TRIGGER_PIN) == 0);
    digitalWrite(LED, HIGH); //light the LED
    for (i = 572; i > 2; i--) //note decrementing a counter is slightly faster
    {
      samples[i] = analogRead(ANALOG_PIN);
    }
  }
  pressure = bmp.readPressure()*100;
  temperature = bmp.readTemperature()*100;
  samples[573] = temperature;
  samples[574] = pressure;
  samples[575] = 9999;
  //Serial.println(millis());
  if (SDCardAvailable)  //Only try to write if the SD Card exists
  {
    File dataFile = SD.open(myfile, FILE_WRITE);
    // SDString = SDString+"\n";
    dataFile.write(samples, 2304);
    dataFile.close(); //close the file
  }
   //Serial.println(millis());

  digitalWrite(LED, LOW);
if (millis()-timecheck>60000) {
  k++;
  Serial.println(myfile);
  timecheck = millis();
}
}
