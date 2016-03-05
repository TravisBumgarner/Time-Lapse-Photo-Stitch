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
  delay(5000);
  
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
