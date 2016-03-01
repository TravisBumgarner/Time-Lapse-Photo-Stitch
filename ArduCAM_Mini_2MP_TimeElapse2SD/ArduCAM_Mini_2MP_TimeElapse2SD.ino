/************************Libraries*************************/
#include <UTFT_SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include <LiquidCrystal.h>

#if defined(__arm__)
#include <itoa.h>
#endif

/**********************Hardware Setup**********************/
//Pins for various elements
//const int buttonUpPin = 2;   *** These buttons will be updated at some point
//const int buttonDownPin = 3; 
//const int buttonContinuePin = 1; 
//const int buttonSetValuePin = 0;

//int buttonUp = LOW;    
//int buttonDown = LOW; 
//int buttonContinue = LOW;
//int buttonSetValue = LOW;

#define SD_CS 9
// set pin 4 as the slave select for SPI:
const int SPI_CS = 3;

ArduCAM myCAM(OV2640, SPI_CS);
UTFT myGLCD(SPI_CS);
LiquidCrystal lcd(10,8,7,6,5,4);

/*******************Various Counters**********************/
int pictureCount = 0; int pictureTotal = 0; //Num of pictures
int hCount = 0; int mCount = 0; int sCount = 0; //Current Time
int hTotal = 0; int mTotal = 0; int sTotal = 0; //Total Time
int intervalTotal = 0;

boolean useQty = true;
boolean useTime = false;


/************************Setup***************************/
void setup()
{
  lcd.begin(16,2);
  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  Serial.begin(115200);
  Serial.println("ArduCAM Start!");
  lcd.setCursor(0,0);
  lcd.print("ArduCAM Start!  ");
  // set the SPI_CS as an output:
  pinMode(SPI_CS, OUTPUT);

  // initialize SPI:
  SPI.begin();
  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    lcd.setCursor(0,1);
    lcd.print("SPI Interface Err");
    Serial.println("SPI interface Error!");
    while (1);
  }

  //Check if the camera module type is OV2640
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  lcd.setCursor(0,1);
  if ((vid != 0x26) || (pid != 0x42)){
    Serial.println("Can't find OV2640 module!");
    lcd.print("2640 is Missing");
  }
  else{
    Serial.println("OV2640 detected");    
    lcd.print("2640 is Found   ");
  }

  //Change to BMP capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);

  myCAM.InitCAM();
  //myCAM.OV2640_set_JPEG_size(OV2640_640x480);
  myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);

  //Initialize SD Card
  if (!SD.begin(SD_CS))
  {
    Serial.println("SD Card Error");
    lcd.setCursor(0,1);
    lcd.print("SD Card Error   ");
    while (1);		//If failed, stop here ***This line might cause errors
  }
  else{
    Serial.println("SD Card detected!");
    lcd.setCursor(0,1);
    lcd.print("SD Card detected");
  }
}

void takePicture()
{
  char str[8];
  File outFile;
  byte buf[256];
  static int i = 0;
  static int k = 0;
  static int n = 0;
  uint8_t temp, temp_last;
  uint8_t start_capture = 0;
  int total_time = 0;
  ////////////////////////////////

  start_capture = 1;
  delay(5000);
  
  if (start_capture)
  {
    //Flush the FIFO
    myCAM.flush_fifo();
    //Clear the capture done flag
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    Serial.println("Start Capture");
    lcd.setCursor(0,1);
    lcd.print("Start Capture   ");
  }

  while (!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));


  Serial.println("Capture Done!");
  lcd.setCursor(0,1);
  lcd.print("Capture Done!   ");
  
  //Construct a file name
  k = k + 1;
  itoa(k, str, 10);
  strcat(str, ".jpg");
  //Open the new file
  outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
  if (! outFile)
  {
    Serial.println("open file failed");
    lcd.setCursor(0,1);
    lcd.print("Open file failed");
    return;
  }
  total_time = millis();
  i = 0;
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  temp = SPI.transfer(0x00);
  //
  //Read JPEG data from FIFO
  while ( (temp != 0xD9) | (temp_last != 0xFF))
  {
    temp_last = temp;
    temp = SPI.transfer(0x00);

    //Write image data to buffer if not full
    if (i < 256)
      buf[i++] = temp;
    else
    {
      //Write 256 bytes image data to file
      myCAM.CS_HIGH();
      outFile.write(buf, 256);
      i = 0;
      buf[i++] = temp;
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
    }
  }
  //Write the remain bytes in the buffer
  if (i > 0)
  {
    myCAM.CS_HIGH();
    outFile.write(buf, i);
  }
  //Close the file
  outFile.close();
  total_time = millis() - total_time;
  Serial.print("Total time used:");
  Serial.print(total_time, DEC);
  Serial.println(" millisecond");
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Clear the start capture flag
  start_capture = 0;
}

void loop(){
  
}


