// anything with *** has to be edited

/************************Libraries*************************/
#include <LiquidCrystal.h>// include the library code

/**********************Hardware Setup**********************/
//Pins for various elements
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
const int buttonUpPin = 8;   
const int buttonDownPin = 9; 
const int buttonContinuePin = 2; 
const int buttonSetValuePin = 3; 

int buttonUp = LOW;    
int buttonDown = LOW; 
int buttonContinue = LOW;
int buttonSetValue = LOW;

//Various Things to be counted
int pictureCount = 0; int pictureTotal = 0; //Num of pictures
int hCount = 0; int mCount = 0; int sCount = 0; //Current Time
int hTotal = 0; int mTotal = 0; int sTotal = 0; //Total Time
int intervalTotal = 0;

/*********************************************************/
void setup()
{
    lcd.begin(16, 2);    // set up the LCD's number of columns and rows:
    Serial.begin(9600);
    pinMode(buttonUpPin, INPUT);
    pinMode(buttonDownPin, INPUT);
    pinMode(buttonContinuePin, INPUT);
    pinMode(buttonSetValuePin, INPUT);
}
/*********************************************************/
void lcdClearLine()
{
    lcd.setCursor(0,0);
    lcd.print("                                ");
    lcd.setCursor(0,1);
    lcd.print("                                ");
} 
/*
void timeUntilNextPhoto(int interval)
{
    for 
    currentTime -= 1;
    delay(1000);
}
*/
    
void screen1(){
    //Setup Screen - Select Time
    while(buttonSetValue != HIGH){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("Select Time:");
        lcd.setCursor(0,1);
        
        int timeSet = 0; //Sets whether time being modified is hours, minutes, or seconds 
        if(buttonSetValue == HIGH){
            break; 
        }
        if(buttonContinue == HIGH){
            if(timeSet == 2){
                timeSet = 0;
            }
            else{
                timeSet++; //Move to next instance
            }
        }
        if(timeSet == 0){ //Set the hours
            if            (buttonUp == HIGH) {hTotal++;}
            else if (buttonDown == HIGH)    {hTotal--;}
        }
        else if(timeSet == 1){ //Set the minutes
            if            (buttonUp == HIGH) {mTotal++;}
            else if (buttonDown == HIGH)    {mTotal--;}
        }
        else if(timeSet == 2){ //Set the seconds
            if            (buttonUp == HIGH) {hTotal++;}
            else if (buttonDown == HIGH)    {hTotal--;}    
        }
        lcd.print(String(hTotal) + "h " 
                        + String(mTotal) + "m " 
                        + String(sTotal) + "s"); //Change to variables
        delay(100);
    }
 
}
void screen2(){
    //Initiatize screen with qty set to true and print that qty is true
    boolean useQty = true;
    boolean useTime = false;
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("Qty or Time?");
    lcd.setCursor(0,1);
    lcd.print(">Qty    Time");  
    while(buttonSetValue != HIGH){
        buttonUp = digitalRead(buttonUpPin);
        buttonDown = digitalRead(buttonDownPin);
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
            delay(150);
        }
    }
    if(useQty){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("What quantity?");
        lcd.setCursor(0,1);
        while(buttonSetValue != HIGH){
            if(buttonUp == HIGH) {pictureTotal++;} //*** Implement use of continue button to go to tens and hundreds column. Maybe just have a display of 0000 total pictures.
            else if (buttonDown == HIGH)    {pictureTotal--;}    
            lcd.print(String(pictureTotal) + "pictures");
            delay(100);
        }
    }
    if(useTime){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("What interval?");
        lcd.setCursor(0,1);
        while(buttonSetValue != HIGH){
            if(buttonUp == HIGH) {intervalTotal++;} //*** Implement use of continue button to go to tens and hundreds column. Maybe just have a display of 0000 total pictures.
            else if (buttonDown == HIGH)    {intervalTotal--;}    
            lcd.print(String(intervalTotal) + "s/pic");
            delay(100);
        }
    }     
}
void screen3(){
    int setupCountdown = 5;
    while(setupCountdown > 0){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("Creating Folder");
        lcd.setCursor(0,1);
        lcd.print("Starting in " + String(setupCountdown) + "s");
        delay(1000);
        setupCountdown--; 
    }     
}
void screen4(int currentSecondOfTimer){
    //Counters for overall pictures and time    
    boolean viewSwitch = false; //Eventually Add physical switch
    if(viewSwitch){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("Next Photo: " + String(currentSecondOfTimer));
    }
    else if(!viewSwitch){
        lcdClearLine();
        lcd.setCursor(0,0);
        lcd.print("P: " + String(pictureCount) + "/" + String(pictureCount)); //Replace numbers with variables
        lcd.setCursor(0,1);
        lcd.print("T: " + String(hCount) + "h " 
                        + String(mCount) + "m " 
                        + String(sCount) + "s");
        if(sCount == 59){
            mCount++;
            sCount = 0;
        }
        else if(sCount<59){
            sCount ++;
        }
        if(mCount == 59){
            hCount++;
            mCount = 0;    
        }
    }
}
void screen5(){
    lcdClearLine();
    lcd.setCursor(0,0);
    lcd.print("Done! P:" + pictureTotal); //Replace numbers with variables
    lcd.setCursor(0,1);
    lcd.print("T: " + String(hTotal) + "h " 
                    + String(mTotal) + "m " 
                    + String(sTotal) + "s");
}
void loop()
{
     screen2();

    /*Serial.println("Serial started.");
    for(int s = 5; s>=0; s--){
        screen4(s);
        delay(1000);
    }
    Serial.println("Picture Taken.");
    pictureCount++;
    */
}

