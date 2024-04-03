#include <LiquidCrystal.h>

const int rs = 12, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Used to determine the contrast of the LCD
const int contrastPin=10;

const int sensPin = A0;

byte hours = 0,minutes = 0,seconds = 0;

float temp;

//Used to count up seconds holds the millis() of the last second increment
unsigned long lastIncrement = 0;

//To edit time
bool editing = false;
const byte editBtn = 11;
const byte modifBtn = 12;
//0 not editing 1 seconds 2 minute 3 hours;
byte editMode = 0;
void setup() {
  Serial.begin(9600);
  Serial.write("test");

  pinMode(editBtn,INPUT);
  pinMode(modifBtn,INPUT);


  lcd.begin(16,2);
  //Shows time text on up left corner
  lcd.setCursor(0,0);
  lcd.print("Time");

  //Shows the temp text on up right corner
  lcd.setCursor(10,0);
  lcd.print("Temp");
}

void timeDisplay(){

  //Shows the time digits below time text
  lcd.setCursor(0,1);
  
  //The ifs are done to ensure that 2 digits are used to represent each thing
  if(hours<10){
    lcd.print(0);
  }
  lcd.print(hours);

  lcd.print(":");

  if(minutes<10){
    lcd.print(0);
  }
  lcd.print(minutes);

  lcd.print(":");
  
  if(seconds<10){
    lcd.print(0);
  }
  lcd.print(seconds);
}

//This function also calls the tempFunc every 1 second
void timeLogic(){
  //Increment second every second
  unsigned long currentTime = millis();
  if(currentTime - lastIncrement >= 1000){
    seconds++;
    lastIncrement = currentTime;
    tempFunc();
  }

  //Calculate minutes
  if(seconds>=60){
    minutes++;
    seconds = 0;
  }

  //Calculate hours
  if(minutes >= 60){
    hours++;
    minutes=0;
  }

  //Reset hours if necessary
  if(hours>=24){
    hours = 0;
  }
}

void time(){
  //If not editing
  if(editMode == 0){
    timeLogic();
    timeDisplay();
  }
}

void tempDisplay(){
  Serial.println(temp);
  char out[5];
  dtostrf(temp,3,1,out);
  lcd.setCursor(10,1);
  lcd.print(out);
}

void tempLogic(){
  // Convert the analog sensor reading into a temperature value in Celsius.
  // This is done by:
  // 1. Reading the analog value from the sensor (0 to 1023).
  // 2. Converting this value to a corresponding voltage (0V to 5V).
  // 3. Adjusting for the sensor's 0.5V offset.
  // 4. Converting the adjusted voltage to temperature in Celsius
  //    (since the sensor has a scale factor of 10mV per degree Celsius).
  temp = (((float)analogRead(sensPin) / 1024.0) * 5.0 - 0.5) * 100;
}
void tempFunc(){
  tempLogic();
  tempDisplay();
}

void chooseMode(){
  //To not instantly go through all modes when button is held
  editMode = (editMode+1)%4;
  Serial.print("Edit mode ");
  Serial.println(editMode);
  delay(200);

  //Put cursor on the current thing being edited
  switch(editMode){
    //Cursor on seconds
    case 1:
      lcd.setCursor(7,1);
      lcd.cursor();
      lcd.blink();
      break;
    //On Minutes
    case 2:
      lcd.setCursor(4,1);
      lcd.cursor();
      lcd.blink();
      break;

    //On Hours
    case 3:
      lcd.setCursor(1,1);
      lcd.cursor();
      lcd.blink();
      break;
    //No cursor since not editing
    default:
      lcd.noCursor();
      lcd.noBlink();
      break;
  }
}

void modifFunc(){
  switch(editMode){
    //Increment seconds
    case 1:
      seconds = (seconds+1)%60;
      //Update display
      timeDisplay();
      delay(50);
      break;
    //Increment moinutes
    case 2:
      minutes = (minutes+1)%60;
      //Update display
      timeDisplay();
      delay(200);
      break;
    //Increment hours
    case 3:
      hours = (hours+1)%60;
      //Update display
      timeDisplay();
      delay(300);
      break;
  }
}
void loop() {
  time();

  if(digitalRead(editBtn)){
    chooseMode();
    // Serial.println("EDITING");
  }
  if(digitalRead(modifBtn)){
    modifFunc();
  }
}
