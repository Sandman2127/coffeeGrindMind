#include <TFT.h>
#include <SPI.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

//LED - 3.3v
//SCK - D13
//SDA - D11
//DC - D9
//Reset - D8
//CS - D10
//GND - GND
//VCC - 5v

unsigned long timeDelay = 0;  // max is 4,294,967,295
unsigned long mSecDelay = 0;
int secDelay = 0;   // max size is -32,768 to 32,767 
int grindStatus = 0;
int anaVal = 0;
int timeDelaySec = 0;
int timeOrWeight = 0;
int grindType = 0;  // 0 timed grind , 1 weigh based grind
int weightMapped = 0; // set weight desired by the user in the function
int setWeight = 0; // set weight desired by the user
int currentWeight = 0; //set by a sensor input read by the arduino, likely going to involve an analogRead()


//pins
int analogP1 = A0;
int analogP2 = A1;
int grinderActivationPin = 4;
int grindStatusPin = 5;
int timeOrWeightPin = 6;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

// char array to print to the screen
char GrindTime[2];  // up to 75 sec
char GrindWeight[3]; // up to 100g
char seconds[5];
char grams[5] ;
int screenWidth = TFTscreen.width();
int screenHeight = TFTscreen.height();
int xPosTime = 35 ;
int yPosTime = screenHeight/3;
int xPosWeight = 35 ;
int yPosWeight =  2*(screenHeight/3);
int x = 0;
int y = 0;

int r_background = 255;
int g_background = 255;
int b_background = 255;

int r_text = 0;
int g_text = 0;
int b_text = 0;


void setup() {
  TFTscreen.begin();
  // clear the screen with a black background
  TFTscreen.background(r_background,g_background,b_background);
  // write the static text to the screen
  // set the font color to white Called before drawing an object on screen, it sets the color of lines and borders around shapes.
  TFTscreen.stroke(r_text, g_text, b_text);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
//  TFTscreen.text("CoffeeGrindMind v0.1\n", screenWidth/8, 5);
  TFTscreen.text("GrindMindv0.1",5,0);
  
  // set the font size very large for the loop
  TFTscreen.setTextSize(3);
  // set the color of the bubbles
  // TFTscreen.fill(255,255,255);
  char seconds[5] = "sec";
  TFTscreen.text(seconds,screenWidth/2,screenHeight/3);
  char grams[5] = "grams";
  TFTscreen.text(grams,screenWidth/2,2*(screenHeight/3));
}


float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

int checkCurrentWeight(){
  return 100;
}

void runWeightBasedGrind(int desiredWeight) {
  // set initial weight
  currentWeight = checkCurrentWeight();
   digitalWrite(grinderActivationPin,HIGH);
  // initiate check loop with 300 msec delay checking the sensor.
  while (currentWeight < desiredWeight) {
  currentWeight = checkCurrentWeight();
  delay(300);
  }
  // shutdown the grinder on closing.
  digitalWrite(grinderActivationPin,LOW);
}

int checkGrindActivationStatus(){
  // push button to 5V above <measure> 4.7k resistor to ground
  grindStatus = digitalRead(grindStatusPin);
  return grindStatus;
}

int checkTimedOrWeightGrind(){
  // toggle switch to 5V above <measure> 4.7k resistor to ground
  // 0 = timed grind, 1 = weight grind
  timeOrWeight = digitalRead(timeOrWeightPin);
  return timeOrWeight; 
}

void runTimedGrind(unsigned long timeDelay) {
  digitalWrite(grinderActivationPin,HIGH);
  delay(timeDelay);
  digitalWrite(grinderActivationPin,LOW);
}

int getSetWeight() {
  anaVal = analogRead(analogP1);
  //  map(value, fromLow, fromHigh, toLow, toHigh)
  weightMapped =  map(anaVal, 0, 1023, 0, 100);
  return weightMapped;
}

unsigned long checkTimer() {
  anaVal = analogRead(analogP1);
  //  map(value, fromLow, fromHigh, toLow, toHigh)
  secDelay =  map(anaVal, 0, 1023, 0, 75);
  mSecDelay = secDelay * 1000;
  return mSecDelay;
}

void writeTimeValue(char GrindTime[2]) {
  TFTscreen.stroke(r_text, g_text, b_text);
  TFTscreen.text(GrindTime, xPosTime, yPosTime);
}

void eraseTimeValue(char GrindTime[2]) {
  TFTscreen.stroke(r_background,g_background,b_background);
  TFTscreen.text(GrindTime, xPosTime, yPosTime);
}

void writeWeightValue(char GrindWeight[3]) {
  TFTscreen.stroke(r_text, g_text, b_text);
  TFTscreen.text(GrindWeight, xPosWeight, yPosWeight);
}

void eraseWeightValue(char GrindWeight[3]) {
  TFTscreen.stroke(r_background,g_background,b_background);
  TFTscreen.text(GrindWeight, xPosWeight, yPosWeight);
}


void loop() {
  // *** MAIN GRINDER RUN LOOP ***
  
  // check grind type, timed: 0, weight based: 1
  grindType = checkTimedOrWeightGrind();
  if (grindType == 0){
    timeDelay = checkTimer();
//    Serial.print("Current timer delay is:");
//    Serial.println(timeDelay);
    // convert time delay from up to 99 sec to character array to print to screen.
    timeDelaySec = timeDelay/1000 ;
    String tString = String(timeDelaySec);
    tString.toCharArray(GrindTime,2);
    writeTimeValue(GrindTime);
    // check if you want immediate grinder activation
    grindStatus = checkGrindActivationStatus();
    if (grindStatus == 1){
      runTimedGrind(timeDelay);
    }
    // basically display the value then erase it and go look for a change...
    delay(750);
    eraseTimeValue(GrindTime);
  } else {
    //weight based grind
    setWeight = getSetWeight();
//    Serial.print("Current weight is selected is: ");
//    Serial.print(setWeight);
//    Serial.println(" grams");
    // convert set weight from up to 999g to character array to print out on screen
    String wString = String(setWeight);
    wString.toCharArray(GrindWeight,3);
    writeWeightValue(GrindWeight);
    
    // check if you want immediate grinder activation
    grindStatus = checkGrindActivationStatus();
    if (grindStatus == 1){
      runWeightBasedGrind(setWeight);
    }
    // basically display the value then erase it and go look for a change...
    delay(750);
    eraseWeightValue(GrindWeight);
  }
}

//void loop() {

//  // Read the value of the sensor on A0
//  String sensorVal = String(analogRead(A0));

//  // convert the reading to a char array
//  sensorVal.toCharArray(sensorPrintout, 4);

//  // set the screen fill color of shapes clear
//  for (int i = 0; i <= 4; i++) {
//    TFTscreen.fill(255,255,255);
//    TFTscreen.stroke(128, 0, 0);
//    x = random(0,screenWidth);
//    y = random(60,screenHeight);
//    // draw a circle in the center of screen
//    TFTscreen.circle(x,y,5);
//    // erase the circle
//    delay(300);
//    TFTscreen.fill(0,0,0);
//    TFTscreen.stroke(0,0,0);
//    TFTscreen.circle(x,y,5);
//    delay(100);
//  }

//  // clear the screen with a black background
//  TFTscreen.background(0,0,0);
//}
