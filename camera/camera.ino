// anything with *** has to be edited

/************************Libraries*************************/
#include <LiquidCrystal.h>// include the library code

/**********************Hardware Setup**********************/
#include <UTFT_SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

#if defined(__arm__)
#include <itoa.h>
#endif

#define SD_CS 8
// set pin 4 as the slave select for SPI:
const int SPI_CS = 9;

ArduCAM myCAM(OV2640, SPI_CS);
UTFT myGLCD(SPI_CS);



//Pins for various elements

LiquidCrystal lcd(7,6,5,4,3,2);

int buttonUp = LOW;    
int buttonDown = LOW; 
int buttonContinue = LOW;
int buttonSetValue = LOW;

//Various Things to be counted
int pictureCount = 0; int pictureTotal = 0; //Num of pictures
int hCount = 0; int mCount = 0; int sCount = 0; //Current Time
int hTotal = 0; int mTotal = 0; int sTotal = 0; //Total Time
int intervalTotal = 0;

//Screen 2 Variables Information
boolean useQty = true;
boolean useTime = false;


/***************************Gahhhhhh Arducam****************/
void setupCamera()
{
  lcd.begin(16,2);
  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  lcd.setCursor(0,0);
  lcd.print("ArduCam Start!  ");
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
    lcd.print("SPI interface Error!");
    while (1);
  }

  //Check if the camera module type is OV2640
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  lcd.setCursor(0,1);
  if ((vid != 0x26) || (pid != 0x42))
  {
    lcd.print("Can't find 2640!");
    while(1);
  }
  else
  {
    lcd.print("2640 detected   "); 
  }
  delay(500); //Let 2640 card info be displayed
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
  //Initialize SD Card
  lcd.setCursor(0,1);
  if (!SD.begin(SD_CS))
  {
    lcd.print("SD Card Error   ");
    while (1);    //If failed, stop here
  }
  else{
    lcd.print("SD Card found!  ");
  }
  delay(500); //Let SD card info be displayed
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
  //delay(5000);
  
  if (start_capture)
  {
    //Flush the FIFO
    myCAM.flush_fifo();
    //Clear the capture done flag
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    lcd.setCursor(0,1);
    lcd.print("Start Capture   ");
  }

  while (!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
    lcd.setCursor(0,1);
    lcd.print("End Capture     ");

  //Construct a file name
  k = k + 1;
  itoa(k, str, 10);
  strcat(str, ".jpg");
  //Open the new file
  outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
  if (! outFile)
  {
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
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Clear the start capture flag
  start_capture = 0;
}
/*********************************************************/
void setup()
{
    setupCamera();
    lcd.begin(16, 2);    // set up the LCD's number of columns and rows:
}
/*********************************************************/
void lcdClearLine()
{
    lcd.setCursor(0,0);
    lcd.print("                                ");
    lcd.setCursor(0,1);
    lcd.print("                                ");
} 

void readInputButtons(){
    int sum;
    buttonUp = LOW;
    buttonDown = LOW;
    buttonContinue =  LOW;
    buttonSetValue =  LOW;
    if      (analogRead(A3)<10){buttonDown = HIGH;}
    else if (analogRead(A2)<10){buttonUp = HIGH;}
    else if (analogRead(A1)<10){buttonContinue = HIGH;}
    else if (analogRead(A0)<10){buttonSetValue = HIGH;}
}
    
void screen1(){
    //Setup Screen - Select Time
    //Serial.println(buttonSetValue);
    int timeSet = 0; //Sets whether time being modified is hours, minutes, or seconds 
    while(buttonSetValue != HIGH){
        readInputButtons();
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("Select Time:");
        lcd.setCursor(13,0);
        if(buttonSetValue == HIGH){
            return;
        }
        if(buttonContinue == HIGH){
            if(timeSet == 2){
                timeSet = 0;
            }
            else if(timeSet < 2){
                timeSet ++ ; //Move to next instance
            }
        }
        if(timeSet == 0){ //Set the hours
            lcd.print("(H)");
            if(buttonUp == HIGH) {hTotal++;}
            else if(buttonDown == HIGH)    {hTotal--;}
        }
        else if(timeSet == 1){ //Set the minutes
            lcd.print("(M)");
            if(buttonUp == HIGH) {mTotal++;}
            else if(buttonDown == HIGH)    {mTotal--;}
        }
        else if(timeSet == 2){ //Set the seconds
            lcd.print("(S)");
            if            (buttonUp == HIGH) {sTotal++;}
            else if (buttonDown == HIGH)    {sTotal--;}    
        }
        lcd.setCursor(0,1);
        lcd.print(String(hTotal) + "h " 
                        + String(mTotal) + "m " 
                        + String(sTotal) + "s"); //Change to variables
        delay(100);
    }
 
}
void screen2(){
  //Used for if useQty is selected
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("What quantity?");
    while(buttonSetValue != HIGH){
        if(buttonSetValue == HIGH){return;}
        readInputButtons();
        if(buttonUp == HIGH) {pictureTotal++;} //*** Implement use of continue button to go to tens and hundreds column. Maybe just have a display of 0000 total pictures.
        else if (buttonDown == HIGH)    {pictureTotal--;}    
        else if (buttonContinue == HIGH) {
          pictureTotal++;
          delay(200);
        }
        lcd.setCursor(0,1);
        lcd.print(String(pictureTotal) + " pictures   ");
        delay(10);
    }
    intervalTotal = (hTotal * 3600 + mTotal * 60 + sTotal) / pictureTotal;
}

void screen3(){
    int setupCountdown = 6;
    while(setupCountdown > 0){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("Creating Folder");
        lcd.setCursor(0,1);
        lcd.print("Starting in " + String(setupCountdown-1) + "s");
        delay(1000);
        setupCountdown--; 
    }     
}
void screen4()
{
    //Counters for overall pictures and time
    boolean viewStats1 = true;
    boolean viewStats2 = false;    
    int intervalCurrent = 0;
    int currentTime = 0;
    int totalTimeInSeconds = hTotal * 3600 + mTotal * 60 + sTotal;
    while (totalTimeInSeconds > currentTime) {
        int timeLeftUntilNextPicture = intervalTotal - intervalCurrent;
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("NP:" + String(timeLeftUntilNextPicture) + "s PicL:" + String(pictureTotal-pictureCount)); //Replace numbers with variables
        lcd.setCursor(0,1);
        /*lcd.print("T: " + String(hCount) + "h " 
                        + String(mCount) + "m " 
                        + String(sCount) + "s");
        if(sCount == 59){mCount++; sCount = 0;}                
        else if(sCount<59){sCount ++;}
        if(mCount == 59){hCount++; mCount = 0;}*/
        intervalCurrent++;
        delay(1000);
        if(timeLeftUntilNextPicture == 0){
          takePicture();
          intervalCurrent = 0;
          pictureCount++;
          if(pictureCount == pictureTotal){
            return;
          }
        } 
    }          
}


void delayAndSetLow(){
  //Clear screen, wait 1 second and read pins.
  lcdClearLine();
  delay(1000);
  readInputButtons();
}

void loop()
{
     screen1();
     delayAndSetLow();
     screen2();
     delayAndSetLow();
     screen3();
     delayAndSetLow();
     screen4();
     lcd.setCursor(0,0);
     lcd.print("Done!           ");
     delay(1000000);
}

