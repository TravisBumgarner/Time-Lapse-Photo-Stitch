// anything with *** has to be edited

/************************Libraries*************************/
#include <LiquidCrystal.h>// include the library code

/**********************Hardware Setup**********************/
//Pins for various elements
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
const int buttonUpPin = 8;   
const int buttonDownPin = 9; 
const int buttonContinuePin = 3; 
const int buttonSetValuePin = 2;
const int camera = 7; 

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

/*********************************************************/
void setup()
{
    lcd.begin(16, 2);    // set up the LCD's number of columns and rows:
    Serial.begin(9600);
    pinMode(buttonUpPin, INPUT);
    pinMode(buttonDownPin, INPUT);
    pinMode(buttonContinuePin, INPUT);
    pinMode(buttonSetValuePin, INPUT);
    pinMode(camera, OUTPUT);
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
    buttonUp = digitalRead(buttonUpPin);
    buttonDown = digitalRead(buttonDownPin);
    buttonContinue = digitalRead(buttonContinuePin);
    buttonSetValue = digitalRead(buttonSetValuePin);  
}
    
void screen1(){
    //Setup Screen - Select Time
    Serial.println(buttonSetValue);
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
            Serial.println("La da de la da da");
            if(timeSet == 2){
                timeSet = 0;
                Serial.println("WHY NOT ME");
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
    //Initiatize screen with qty set to true and print that qty is true
    delay(150); //***  ONCE DONE CODING SCREEN ONE
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("Qty or Time?");
    lcd.setCursor(0,1);
    lcd.print(">Qty    Time");  
    while(buttonSetValue != HIGH){
        delay(500); //*** Maybe remove once done.
        readInputButtons();
        if(buttonSetValue == HIGH){
            return; 
        }
        lcd.setCursor(0,1);
        if(buttonContinue == HIGH){
            if(useQty == true){
                useQty = false;
                useTime = true;
                lcd.print(" Qty    >Time   ");     
            }
            else if(useTime == true){
                useQty = true;
                useTime = false;
                lcd.print(">Qty     Time   ");     
            }
        }
    }
}

void screen21(){
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
        intervalTotal = hTotal * 3600 + mTotal * 60 + sTotal / pictureTotal; //*** Hope this works the first time.
        delay(10);
    }
}

void screen22(){
  //Used for if useTime is selected
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("What duration?");
    while(buttonSetValue != HIGH){
        readInputButtons();
        if(buttonSetValue == HIGH){return;}
        if(buttonUp == HIGH) {intervalTotal++;} //*** Implement use of continue button to go to tens and hundreds column. Maybe just have a display of 0000 total pictures.
        else if (buttonDown == HIGH)    {intervalTotal--;}    
        else if (buttonContinue == HIGH){
          intervalTotal++;
          delay(200);
        }
        lcd.setCursor(0,1);
        lcd.print(String(intervalTotal) + "s     ");
        pictureTotal = hTotal * 3600 + mTotal * 60 + sTotal / intervalTotal; //*** Hope this works first time around.
        delay(100); //Take some time to set the number of seconds
    }
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
void screen4(){
    //Counters for overall pictures and time
    boolean viewStats1 = true;
    boolean viewStats2 = false;    
    int intervalCurrent = 0;
    int currentTime = 0;
    int totalTimeInSeconds = hTotal * 3600 + mTotal * 60 + sTotal;
    while (totalTimeInSeconds > currentTime) {
        int timeLeftUntilNextPicture = intervalTotal - intervalCurrent;
        readInputButtons();
        if(buttonContinue == HIGH){
            if(viewStats1 == true){
                viewStats1 = false;
                viewStats2 = true;    
            }
            else if(viewStats2 == true){
                viewStats1 = true;
                viewStats2 = false;
            } 
        }
        if(viewStats1){
                lcdClearLine();
                lcd.setCursor(0,0);
                lcd.print("Next Photo: " + String(timeLeftUntilNextPicture));
            }
        else if(viewStats2){
            lcdClearLine();
            lcd.setCursor(0,0);
            lcd.print("P: " + String(pictureCount) + "/" + String(pictureTotal)); //Replace numbers with variables
            lcd.setCursor(0,1);
            lcd.print("T: " + String(hCount) + "h " 
                            + String(mCount) + "m " 
                            + String(sCount) + "s");
        }
        for(int s = 1; s<=20; s++){
            readInputButtons();
            delay(50);
        }
        if(sCount == 59){mCount++; sCount = 0;}                
        else if(sCount<59){sCount ++;}
        if(mCount == 59){hCount++; mCount = 0;}
        intervalCurrent++; 
        if(timeLeftUntilNextPicture == 0){
          digitalWrite(camera,HIGH);
          delay(2000);
          digitalWrite(camera,LOW);
          intervalCurrent = 0;
          pictureCount++;
          Serial.println(String(pictureCount) + "count and total" + String(pictureTotal));
          if(pictureCount == pictureTotal){
            return;
          }
        }             
    }
}
void screen5(){
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("Done! P:" + String(pictureTotal)); //Replace numbers with variables
    lcd.setCursor(0,1);
    lcd.print("T: " + String(hTotal) + "h " 
                    + String(mTotal) + "m " 
                    + String(sTotal) + "s");
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
     Serial.println("Using " + String(hTotal) + "h" + String(mTotal) + "m" + String(sTotal) + "s");
     screen2();
     delayAndSetLow();
     if(useQty == true){
        screen21();
     }
     else if(useTime == true){
        screen22();
     }
     delayAndSetLow();
     screen3();
     delayAndSetLow();
     screen4();
     delayAndSetLow();
     screen5();
     delay(1000000);
}

