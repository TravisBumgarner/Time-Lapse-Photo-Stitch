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
int buttonUp = LOW;
int buttonDown = LOW;
int buttonContinue = LOW;
int buttonSetValue = LOW;
#define SD_CS 9
// set pin 4 as the slave select for SPI:
const int SPI_CS = 3;
ArduCAM myCAM(OV2640, SPI_CS);
UTFT myGlcd(SPI_CS);
LiquidCrystal lcd(10, 8, 7, 6, 5, 4);
/*******************Various Counters**********************/
int pictureCount = 0; int pictureTotal = 0; //Num of pictures
int hCount = 0; int mCount = 0; int sCount = 0; //Current Time
int hTotal = 0; int mTotal = 0; int sTotal = 0; //Total Time
int intervalTotal = 0;
boolean useQty = true;
boolean useTime = false;
/**************************Setup***************************/
void setup()
{
  lcd.begin(16, 2);
  //Serial.begin(9600);
  uint8_t vid, pid;
  uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  //Serial.begin(115200);
  //Serial.println("ArduCAM Start!");
  lcd.setCursor(0, 0);
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
    lcd.setCursor(0, 1);
    lcd.print("SPI Interface Err");
    //Serial.println("SPI interface Error!");
    while (1);
  }
  //Check if the camera module type is OV2640
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  lcd.setCursor(0, 1);
  if ((vid != 0x26) || (pid != 0x42)) {
    //Serial.println("Can't find OV2640 module!");
    lcd.print("2640 is Missing  ");
  }
  else {
    //Serial.println("OV2640 detected");
    lcd.print("2640 is Found   ");
  }
  delay(1000);
  //Change to BMP capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  //myCAM.OV2640_set_JPEG_size(OV2640_640x480);
  //myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);

  //Initialize SD Card
  if (!SD.begin(SD_CS))
  {
    //Serial.println("SD Card Error");
    lcd.setCursor(0, 1);
    lcd.print("SD Card Error   ");
    while (1);		//If failed, stop here ***This line might cause errors
  }
  else {
    //Serial.println("SD Card detected!");
    lcd.setCursor(0, 1);
    lcd.print("SD Card detected");
  }
  delay(1000);
}
/**************************Functions**************************/
void lcdClearLine() //This was replaced with just making every string 16 characters long
  {
  lcd.setCursor(0, 0);
  lcd.print("                                ");
  lcd.setCursor(0, 1);
  lcd.print("                                ");
  }
void readInputButtons() {
  int sum;
  buttonUp = LOW;
  buttonDown = LOW;
  buttonContinue =  LOW;
  buttonSetValue =  LOW;
  if      (analogRead(A0) < 10) {
    buttonDown = HIGH;
  }
  else if (analogRead(A1) < 10) {
    buttonUp = HIGH;
  }
  else if (analogRead(A2) < 10) {
    buttonContinue = HIGH;
  }
  else if (analogRead(A3) < 10) {
    buttonSetValue = HIGH;
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
    //Serial.println("Start Capture");
    lcd.setCursor(0,1);
    lcd.print("Start Capture    ");
  }

  while (!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));


  //Serial.println("Capture Done!");
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
    //Serial.println("open file failed");
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
  //Serial.print("Total time used:");
  //Serial.print(total_time, DEC);
  //Serial.println(" millisecond");
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Clear the start capture flag
  start_capture = 0;
}

void screen1() {
  //Setup Screen - Select Time
  ////Serial.println(buttonSetValue);
  int timeSet = 0; //Sets whether time being modified is hours, minutes, or seconds
  while (buttonSetValue != HIGH) {
    readInputButtons();
    lcdClearLine();
    lcd.setCursor(0, 0);
    lcd.print("Select Time: ");
    lcd.setCursor(13, 0);
    if (buttonSetValue == HIGH) {
      return;
    }
    if (buttonContinue == HIGH) {
      if (timeSet == 2) {
        timeSet = 0;
      }
      else if (timeSet < 2) {
        timeSet ++ ; //Move to next instance
      }
    }
    if (timeSet == 0) { //Set the hours
      lcd.print("(H)");
      if (buttonUp == HIGH) {
        hTotal++;
      }
      else if (buttonDown == HIGH)    {
        hTotal--;
      }
    }
    else if (timeSet == 1) { //Set the minutes
      lcd.print("(M)");
      if (buttonUp == HIGH) {
        mTotal++;
      }
      else if (buttonDown == HIGH)    {
        mTotal--;
      }
    }
    else if (timeSet == 2) { //Set the seconds
      lcd.print("(S)");
      if            (buttonUp == HIGH) {
        sTotal++;
      }
      else if (buttonDown == HIGH)    {
        sTotal--;
      }
    }
    lcd.setCursor(0, 1);
    lcd.print(String(hTotal) + "h "
                  + String(mTotal) + "m "
                  + String(sTotal) + "s         "); //Change to variables
    delay(50);
  }
}
void screen2() {
  //Initiatize screen with qty set to true and print that qty is true
  // lcdClearLine();
  lcd.setCursor(0, 0);
  lcd.print("Qty or Time?    ");
  lcd.setCursor(0, 1);
  lcd.print(">Qty    Time    ");
  while (buttonSetValue != HIGH) {
    delay(500); //*** Maybe remove once done.
    readInputButtons();
    if (buttonSetValue == HIGH) {
      return;
    }
    lcd.setCursor(0, 1);
    if (buttonContinue == HIGH) {
      if (useQty == true) {
        useQty = false;
        useTime = true;
        lcd.print(" Qty    >Time   ");
      }
      else if (useTime == true) {
        useQty = true;
        useTime = false;
        lcd.print(">Qty     Time   ");
      }
    }
  }
}
void screen21() {
  //Used for if useQty is selected
  lcdClearLine();
  lcd.setCursor(0, 0);
  lcd.print("What quantity?  ");
  while (buttonSetValue != HIGH) {
    if (buttonSetValue == HIGH) {
      return;
    }
    readInputButtons();
    if (buttonUp == HIGH) {
      pictureTotal++; //*** Implement use of continue button to go to tens and hundreds column. Maybe just have a display of 0000 total pictures.
    }
    else if (buttonDown == HIGH)    {
      pictureTotal--;
    }
    else if (buttonContinue == HIGH) {
      pictureTotal++;
      delay(200);
    }
    lcd.setCursor(0, 1);
    lcd.print(String(pictureTotal) + " pictures       ");
    delay(10);
  }
  intervalTotal = (hTotal * 3600 + mTotal * 60 + sTotal) / pictureTotal; //*** Hope this works the first time.
  //Serial.println(String(intervalTotal));
}
void screen22() {
  //Used for if useTime is selected
  lcdClearLine();
  lcd.setCursor(0, 0);
  lcd.print("What duration?  ");
  while (buttonSetValue != HIGH) {
    readInputButtons();
    if (buttonSetValue == HIGH) {
      return;
    }
    if (buttonUp == HIGH) {
      intervalTotal++; //*** Implement use of continue button to go to tens and hundreds column. Maybe just have a display of 0000 total pictures.
    }
    else if (buttonDown == HIGH)    {
      intervalTotal--;
    }
    else if (buttonContinue == HIGH) {
      intervalTotal++;
      delay(200);
    }
    lcd.setCursor(0, 1);
    lcd.print(String(intervalTotal) + "s     ");
    pictureTotal = (hTotal * 3600 + mTotal * 60 + sTotal) / intervalTotal; //*** Hope this works first time around.
    delay(100); //Take some time to set the number of seconds
  }
}
void screen3() {
  lcd.setCursor(0, 0);
  lcd.print("Starting in 5s");
  delay(5000);
}

void screen4() {
  //Counters for overall pictures and time
  boolean viewStats1 = true;
  boolean viewStats2 = false;
  int intervalCurrent = 0;
  int currentTime = 0;
  int totalTimeInSeconds = hTotal * 3600 + mTotal * 60 + sTotal;
  while (totalTimeInSeconds > currentTime) {
    int timeLeftUntilNextPicture = intervalTotal - intervalCurrent;
    readInputButtons();
    if (buttonContinue == HIGH) {
      if (viewStats1 == true) {
        viewStats1 = false;
        viewStats2 = true;
      }
      else if (viewStats2 == true) {
        viewStats1 = true;
        viewStats2 = false;
      }
    }
    if (viewStats1) {
      lcdClearLine();
      lcd.setCursor(0, 0);
      lcd.print("Next Photo: " + String(timeLeftUntilNextPicture)+"s");
    }
    else if (viewStats2) {
      lcdClearLine();
      lcd.setCursor(0, 0);
      lcd.print("P: " + String(pictureCount) + "/" + String(pictureTotal)+"     "); //Replace numbers with variables
      lcd.setCursor(0, 1);
      lcd.print("T: " + String(hCount) + "h "
                      + String(mCount) + "m "
                    + String(sCount) + "s"+ "     ");
    }
    for (int s = 1; s <= 20; s++) {
      readInputButtons();
      delay(50);
    }
    if (sCount == 59) {
      mCount++;
      sCount = 0;
    }
    else if (sCount < 59) {
      sCount ++;
    }
    if (mCount == 59) {
      hCount++;
      mCount = 0;
    }
    intervalCurrent++;
    if (timeLeftUntilNextPicture == 0) {
      takePicture();
      intervalCurrent = 0;
      pictureCount++;
      if (pictureCount == pictureTotal) {
        return;
      }
    }
  }
}
/*void screen5(){
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("Done! P:" + String(pictureTotal)); //Replace numbers with variables
    lcd.setCursor(0,1);
    lcd.print("T: " + String(hTotal) + "h "
                    + String(mTotal) + "m "
                    + String(sTotal) + "s");
  }*/
void delayAndSetLow() {
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
  if (useQty == true) {
    screen21();
  }
  else if (useTime == true) {
    screen22();
  }
  delayAndSetLow();
  screen3();
  delayAndSetLow();
  screen4();
  delayAndSetLow();
  // screen5();
  delay(1000000);
}
