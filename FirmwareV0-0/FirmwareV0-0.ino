/*

  Lite3DP S1 v0.0

  https://github.com/Lite3DP/Lite3DP-S1

  MIT License
 
  Please visit www.lite3dp.com for more information, documentation and software.

  email: lite3dp@gmail.com

*/


/*
 
***SELECT TO UPLOAD***

BOARD: Arduino Pro or Pro Mini

PROCESSOR: ATmega328P (5V, 16 MHz)

*/


/*
 
  Libraries used:

  SdFat: https://github.com/greiman/SdFat, MIT license

  Adafruit_GFX_AS and Adafruit_ILI9341_AS: https://github.com/Bodmer/TFT_ILI9341 (newest version)

  Original Adafruit library: https://github.com/adafruit/Adafruit_ILI9341

*/


/*

  This is a simple program so that anyone can make changes, upgrades and customizations.

  It is basically structured around a system of numbered "screens" for menu navigation.
  Depending on the screen, the buttons will have different functions. 

  The main functions are "print" and "calibrate".

  The possibility of customizing the motor, the final height and the threaded rod is contemplated ("For customization" section).

  In the configuration section you can change some values such as the lift distance and the number of initial layers.

*/


////////////////////////////////////////////////////////////

// ***** FOR CUSTOMIZATION ****

// If you want to change the length of the linear guide, the threaded rod, the motor or the microsteps
// change them here and the rest of the variables will be updated.


#define Lguide 100               //Total length of linear guide (Default value : 100 mm)

#define pitch 2                  //Pitch of movement screw thread (Default value : 2 mm)

#define stepsMotorPerRev 720     //Motor steps per revolution, including gear reduction (Default value : 720 steps/rev)

#define microsteps 1             //Driver microsteps (Default value 1)


////////////////////////////////////////////////////////////

// **** CONFIGURATION PARAMETERS ****


#define hUp 3                   // Lift distance in mm (Default value: 3 mm)

#define FirstLayers 5           //Number of initial layers (Default value: 5 initial layers)

int expotime = 15;              //Starting exposure time in seconds

int iexpotime = 45;             //Starting initial exposure time in seconds

#define HighSpeed 1800          //Delay in microseconds of the stepper motor pulses; higher speed at lower value (Default = 1800 microseconds) 

#define LowSpeed 2500           //Delay in microseconds of the stepper motor pulses; higher speed at lower value (Default = 2500 microseconds)

#define updowntime 7.3          //Printing downtime in seconds. Experimental value used to calculate print times. Counted from turning off the UV light on one layer and turning on the UV light on the next one. 
                                //It must be modified if any of the configuration parameters are changed (speeds, number of initial layers, lift distance).


////////////////////////////////////////////////////////////

// Pin definition

#define PinDir 15
#define PinStep 14
#define PinEn 10

#define sclk 13
#define miso 12
#define mosi 11
#define cs   4
#define dc   3
#define rst  2

#define _sdcs 5

#define BtnUp 9
#define BtnDown 6
#define BtnOK 7
#define BtnCancel 8

#define PinEndStop 17

#define LightPin 16


///////////////////////////////////////////////

// ****LIBRARIES****

#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <SPI.h>
#include <EEPROM.h>

#include <SdFat.h>
SdFat SD;

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(cs, dc, rst);
#define BU_BMP 1 // Temporarily flip the TFT coords for standard Bottom-Up bit maps
#define TD_BMP 0 // Draw inverted Top-Down bitmaps in standard coord frame


///////////////////////////////////////////////

// ****GLOBAL VARIABLES****

// **Screen number**

byte screen = 1;


//**Button status**

bool edoBtnUP = HIGH;
bool edoBtnDOWN = HIGH;
bool edoBtnOK = HIGH;
bool edoBtnCANCEL = HIGH;


//**Button delay**

#define delaybutton 90


// **EndStop Lecture**

bool LectEndStop;


// **Layer Height**

float hLayer = 0.050;
int hLayerx1000 = hLayer * 1000;


// **Ascendant movement Height (mm)**

int maxheight = Lguide - 30;          //(30 mm lost due to carriage and platform)


// **Print descendant movement**

float hDown = hUp - hLayer;


// **Steps per mm**

int StepsPerMm = stepsMotorPerRev * microsteps / pitch;


// **Calibration aditional steps**

#define maxAddDesc 3     // Additional maximum descent (Default = 3 mm)

int maxAddSteps = maxAddDesc * StepsPerMm / 8;  //Division by 8 is due to memory EEPROM; then it is multiplied again.

byte stepsadditional = 0;

int stepsadditionalx8;


// **For the correct name of file**

String DirAndFile;

String FileName;

char *result = malloc(5);

int number;


//**For the layers counter**

int LayersCounter = 0;

int Layers;

String dirfoldersel;


// **For the folder´s name**

File root;

char foldersel[13];

int counter = 1;


///////////////////////////////////////////////



void setup(void) {


  pinMode (BtnUp, INPUT_PULLUP);
  pinMode (BtnDown, INPUT_PULLUP);
  pinMode (BtnCancel, INPUT_PULLUP);
  pinMode (BtnOK, INPUT_PULLUP);

  pinMode (PinDir, OUTPUT);
  pinMode (PinStep, OUTPUT);
  pinMode (PinEn, OUTPUT);

  pinMode (LightPin, OUTPUT);

  pinMode (PinEndStop, INPUT);


  digitalWrite (LightPin, LOW);
  digitalWrite (PinEn, HIGH);

  tft.init();

  screenlite3dp();  //Opening screen

  screen1();   //First menu screen

}


////////////////////////////////////////////////////

void loop() {

  edoBtnUP = digitalRead (BtnUp);
  edoBtnDOWN = digitalRead (BtnDown);
  edoBtnOK = digitalRead (BtnOK);
  edoBtnCANCEL = digitalRead (BtnCancel);

  if (edoBtnUP == LOW) {

    switch (screen) {

      case 1:
        movasc(0.05, HighSpeed);    //Allows the possibility of raising the platform
        break;

      case 11:
        folderUp(root);
        delaybtn();
        break;

      case 12:
        switch (hLayerx1000) {
          case 50:
            hLayer = 0.1;
            hLayerx1000 = hLayer * 1000;
            String hLayerString = String(hLayer);
            char hLayerChar[7];
            hLayerString.toCharArray(hLayerChar, 7);
            tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
            tft.drawCentreString(hLayerChar, 161, 115, 2);
            adjuststeps();
            break;
        }
        delaybtn();
        break;


      case 13:
        if (expotime >= 0 && screen == 13) {
          expotime++;
          String expotimeString = String(expotime);
          char expotimeChar[7];
          expotimeString.toCharArray(expotimeChar, 7);
          tft.fillRect(122, 112, 76, 36, ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);
          tft.drawCentreString(expotimeChar, 161, 115, 2);
        }
        iexpotime = 3 * expotime;   // initial approximation
        delaybtn();
        break;

      case 14:
        if (iexpotime >= 0 && screen == 14) {
          iexpotime++;
          String iexpotimeString = String(iexpotime);
          char iexpotimeChar[7];
          iexpotimeString.toCharArray(iexpotimeChar, 7);
          tft.fillRect(122, 112, 76, 36, ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);
          tft.drawCentreString(iexpotimeChar, 161, 115, 2);
        }
        delaybtn();
        break;


      case 21:                                   //screen 21 is when the platform is down during calibration
        if (stepsadditional > 0) {
          stepsadditional--;
          digitalWrite(PinDir, HIGH);
          digitalWrite (PinEn, LOW);
          for (int z = 0; z < 8; z++) {
            edoBtnCANCEL = digitalRead (BtnCancel);
            digitalWrite(PinStep, HIGH);
            digitalWrite(PinStep, LOW);
            delayMicroseconds(LowSpeed);
          }
          digitalWrite (PinEn, HIGH);
        }
        break;

    }

  }

  if (edoBtnDOWN == LOW) {

    switch (screen) {

      case 1:
        break;

      case 11:
        folderDown(root);
        delaybtn();
        break;

      case 12:
        switch (hLayerx1000) {
          case 100:
            hLayer = 0.05;
            hLayerx1000 = hLayer * 1000;
            String hLayerString = String(hLayer);
            char hLayerChar[7];
            hLayerString.toCharArray(hLayerChar, 7);
            tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
            tft.drawCentreString(hLayerChar, 161, 115, 2);
            adjuststeps();
            break;
        }
        delaybtn();
        break;

      case 13:
        if (expotime > 0 && screen == 13) {
          expotime--;
          String expotimeString = String(expotime);
          char expotimeChar[7];
          expotimeString.toCharArray(expotimeChar, 7);
          tft.fillRect(122, 112, 76, 36, ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);
          tft.drawCentreString(expotimeChar, 161, 115, 2);
        }
        iexpotime = 3 * expotime;   // initial approximation
        delaybtn();
        break;

      case 14:
        if (iexpotime > 0 && screen == 14) {
          iexpotime--;
          String iexpotimeString = String(iexpotime);
          char iexpotimeChar[7];
          iexpotimeString.toCharArray(iexpotimeChar, 7);
          tft.fillRect(122, 112, 76, 36, ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);
          tft.drawCentreString(iexpotimeChar, 161, 115, 2);
        }
        delaybtn();
        break;

      case 21:                                   //screen 21 is when the platform is down during calibration
        if (stepsadditional <= maxAddSteps) {
          stepsadditional++;
          digitalWrite(PinDir, LOW);
          digitalWrite (PinEn, LOW);
          for (int z = 0; z < 8; z++) {
            edoBtnCANCEL = digitalRead (BtnCancel);
            digitalWrite(PinStep, HIGH);
            digitalWrite(PinStep, LOW);
            delayMicroseconds(LowSpeed);
          }
          digitalWrite (PinEn, HIGH);
        }
        break;

    }

  }


  if (edoBtnOK == LOW) {

    switch (screen) {

      case 1:
        blackscreen();
        calibrate();
        screen = 21;
        break;

      case 11:
        contarlayers();
        if (LayersCounter > maxheight * 20 + 3) {
          cleanscreen();
          tft.drawCentreString("HEIGHT EXCEEDED", 160, 115, 2);
          delay(1200);
          screen = 11;
          screen11();
        }

        else {
          screen = 12;
          screen12();
        }
        break;

      case 12:
        screen = 13;
        screen13();
        break;

      case 13:
        screen = 14;
        screen14();
        break;

      case 14:
        screen = 15;
        screen15();
        break;

      case 15:
        switch (hLayerx1000) {

          case 50:
            Layers = LayersCounter;
            break;

          case 100:
            Layers = LayersCounter / 2;

            int resto = Layers * 2;
            resto = LayersCounter - resto;

            if (resto > 0) {
              Layers++;
            }

            break;
        }

        screen = 16;
        screen16();
        break;

      case 16:
        blackscreen();
        print();
        break;

      case 21:
        EEPROM.write (11, stepsadditional);
        movasc(50, HighSpeed);                    // 50 mm lift to make room for reading the display and tray entry.
        if (!SD.begin(_sdcs, SPI_FULL_SPEED)) {
          digitalWrite (LightPin, LOW);
          cleanscreen();
          tft.drawCentreString("Please", 160, 65, 2);
          tft.drawCentreString("insert SD card", 160, 100, 2);
          screen = 22;
        }
        else {
          digitalWrite (LightPin, LOW);
          root = SD.open("/");
          screen11();
          screen = 11;
        }
        break;

      case 22:
        if (!SD.begin(_sdcs, SPI_FULL_SPEED)) {
          digitalWrite (LightPin, LOW);
          cleanscreen();
          tft.drawCentreString("Please", 160, 65, 2);
          tft.drawCentreString("insert SD card", 160, 100, 2);
          screen = 22;
        }
        else {
          digitalWrite (LightPin, LOW);
          root = SD.open("/");
          screen11();
          screen = 11;
        }
        break;


    }

    delaybtn();

  }

  if (edoBtnCANCEL == LOW) {


    if (screen < 17 && screen > 11 ) {
      screen = 11;
      screen11();
    }

    else {
      screen = 1;
      screen1();
    }
    delaybtn();
  }


}

////////////////////////////////////////////////////////////


//***** MAIN FUNCTIONS *****

// PRINT FUNCTION

void print() {

  number = 1;
  buildfolder();
  calibrate();

  // Printing the first layers (with initial exposure time)

  for (int l = 0; l < FirstLayers; l++) {

    printname();
    digitalWrite (LightPin, HIGH);
    delayprint1();              
    delayprint1();
    delayprint1();
    delayprint1();
    digitalWrite (LightPin, LOW);
    blackscreen();
    pause();                         //allows to enter the pause function by holding down the ESC button
    movasc(hUp, LowSpeed);
    delay(200);
    movdesc(hDown, LowSpeed);

  }

  // Printing the rest

  for (int l = 0; l < Layers - FirstLayers; l++) {

    printname();
    digitalWrite (LightPin, HIGH);
    delayprint2();
    delayprint2();
    delayprint2();
    delayprint2();
    digitalWrite (LightPin, LOW);
    blackscreen();
    pause();                         //allows to enter the pause function by holding down the ESC button
    movasc(hUp, LowSpeed);
    delay(200);
    movdesc(hDown, LowSpeed);

  }

  screen = 17;
  screen17();

}

///////////////////////

// CALIBRATION FUNCTION

void calibrate() {

  LectEndStop = digitalRead(PinEndStop);

  if (LectEndStop != HIGH) {

    desctoendstop();

    delay(600);
    stepsadditional = EEPROM.read(11);
    stepsadditionalx8 = stepsadditional * 8;

    digitalWrite (PinEn, LOW);
    digitalWrite(PinDir, LOW);

    for (int z = 0; z < stepsadditionalx8; z++) {
      edoBtnCANCEL = digitalRead (BtnCancel);
      digitalWrite(PinStep, HIGH);
      digitalWrite(PinStep, LOW);
      delayMicroseconds(LowSpeed);
      if (edoBtnCANCEL == LOW) {
        break;
      }
    }

    digitalWrite (PinEn, HIGH);
  }
}

/////////////////////////////////////////////////////////////

// CALIBRATION AND PRINT SUPPORT FUNCTIONS

void folderDown(File dir) {
  counter++;
  for (int i = 0; i < counter; i++) {
    while (true) {
      File entry =  dir.openNextFile();
      if (! entry) {
        break;
      }
      if (entry.isDirectory()) {
        entry.getName(foldersel, 13);
        break;
      }
      entry.close();
    }
  }
  tft.fillRect(78, 130, 164, 36, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString(foldersel, 161, 134, 2);
  delay(200);

}

void folderUp(File dir) {
  if (counter > 2) {
    counter --;
    for (int i = 0; i < counter; i++) {
      while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
          break;
        }
        if (entry.isDirectory()) {
          entry.getName(foldersel, 13);
          break;
        }
        entry.close();
      }
    }
    tft.fillRect(78, 130, 164, 36, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.drawCentreString(foldersel, 161, 134, 2);
    delay(200);
  }

}


void movasc (float Mm, int delaysteps) {

  long int stepsmotor = StepsPerMm * Mm;

  digitalWrite (PinEn, LOW);
  digitalWrite(PinDir, HIGH);

  for (long int x = 0; x < stepsmotor; x++) {
    edoBtnCANCEL = digitalRead (BtnCancel);
    digitalWrite(PinStep, HIGH);
    digitalWrite(PinStep, LOW);
    delayMicroseconds(delaysteps);
  }

  digitalWrite (PinEn, HIGH);
}


void movdesc (float Mm, int delaysteps) {

  long int stepsmotor = StepsPerMm * Mm;

  digitalWrite (PinEn, LOW);
  digitalWrite(PinDir, LOW);

  for (long int x = 0; x < stepsmotor; x++) {
    edoBtnCANCEL = digitalRead (BtnCancel);
    digitalWrite(PinStep, HIGH);
    digitalWrite(PinStep, LOW);
    delayMicroseconds(delaysteps);
  }
  digitalWrite (PinEn, HIGH);
}


void buildfolder() {

  DirAndFile = "";
  String folderselString = String(foldersel);
  String barra = "/";
  DirAndFile += barra;
  DirAndFile += folderselString;
  DirAndFile += barra;
  FileName = DirAndFile;

}

void contarlayers() {

  LayersCounter = 0;
  dirfoldersel = "";
  String folderselString2 = String(foldersel);
  number = 1;
  dirfoldersel += "/";
  dirfoldersel += folderselString2;
  dirfoldersel += "/";
  char dirfolderselChar[20];
  dirfoldersel.toCharArray(dirfolderselChar, 20);
  File dircarp = SD.open(dirfolderselChar);
  while (true) {
    File entry =  dircarp.openNextFile();
    if (! entry) {
      break;
    }
    if (entry.isDirectory()) {
    } else {
      LayersCounter ++;
    }
    entry.close();
  }
}


void delayprint1() {
  delay (iexpotime * 250);
}


void delayprint2() {
  delay (expotime * 250);
}


void printname() {
  FileName += number;
  FileName += ".bmp";
  char NameChar[20];
  FileName.toCharArray(NameChar, 20);
  tft.setRotation(3);
  drawBMP(NameChar, 0, 0, BU_BMP);
  switch (hLayerx1000) {
    case 50:
      number ++;
      break;

    case 100:
      number = number + 2;
      break;
  }
  FileName = DirAndFile;
}


void pause() {

  edoBtnOK = digitalRead (BtnOK);
  edoBtnCANCEL = digitalRead (BtnCancel);

  if (edoBtnCANCEL == LOW) {

    float heightActual = number * 0.05;
    float heightAdd = maxheight - heightActual;
    movasc(heightAdd, HighSpeed);
    while (digitalRead (BtnOK) != LOW) {
    }
    movdesc(heightAdd, HighSpeed);
    delaybtn();

  }

}



void desctoendstop() {

  digitalWrite (PinEn, LOW);
  digitalWrite(PinDir, LOW);
  LectEndStop = digitalRead(PinEndStop);

  //DESCENSO HASTA ENDSTOP
  while (LectEndStop != HIGH) {
    LectEndStop = digitalRead(PinEndStop);
    edoBtnCANCEL = digitalRead (BtnCancel);
    digitalWrite(PinStep, HIGH);
    digitalWrite(PinStep, LOW);
    delayMicroseconds(HighSpeed);
  }
  delay(300);
}


void delaybtn() {
  delay(delaybutton);
}


void adjuststeps() {

  hDown = hUp - hLayer;

}


////////////////////////////////////////////////////////////

// SCREENS


void screenlite3dp() {

  tft.setRotation(3);
  tft.setTextSize(2);
  blackscreen();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("Lite3DP", 160, 100, 2);
  tft.fillTriangle(123, 104, 134, 104, 129, 114, ILI9341_BLUE);
  delay(2500);

}


void screen1() {

  tft.setRotation(3);
  cleanscreen();
  bannerpreparation();
  tft.drawCentreString("Prepare!", 160, 120, 2);

}


void screen11() {

  bannerprint();
  cleanscreen();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("SELECT FILE", 160, 65, 2);
  tft.drawCentreString(foldersel, 161, 134, 2);
  tft.drawRect(75, 123, 170, 50, ILI9341_WHITE);
  arrows();

}


void screen12() {

  cleanscreen();
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.drawCentreString("LAYER HEIGHT", 160, 60, 2);
  String hLayerString = String(hLayer);
  char hLayerChar[7];
  hLayerString.toCharArray(hLayerChar, 7);
  tft.drawCentreString(hLayerChar, 161, 115, 2);
  tft.drawCentreString("mm", 161, 160, 2);
  rectscreen();
  arrows();

}


void screen13() {

  cleanscreen();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("EXPOSURE TIME", 160, 60, 2);
  String expotimeString = String(expotime);
  char expotimeChar[7];
  expotimeString.toCharArray(expotimeChar, 7);
  tft.drawCentreString(expotimeChar, 161, 115, 2);
  tft.drawCentreString("sec", 161, 160, 2);
  rectscreen();
  arrows();

}


void screen14() {

  cleanscreen();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("INITIAL EXPOSURE", 160, 60, 2);
  String iexpotimeString = String(iexpotime);
  char iexpotimeChar[7];
  iexpotimeString.toCharArray(iexpotimeChar, 7);
  tft.drawCentreString(iexpotimeChar, 161, 115, 2);
  tft.drawCentreString("sec", 161, 160, 2);
  rectscreen();
  arrows();
}


void screen15() {

  cleanscreen();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("CONFIRMATION", 160, 55, 2);

  tft.drawString("FOLDER:", 20, 100, 2);
  tft.drawString(foldersel, 220, 100, 2);

  tft.drawString("LAYER H.:", 20, 130, 2);
  String hLayerString = String(hLayer);
  char hLayerChar[7];
  hLayerString.toCharArray(hLayerChar, 7);
  tft.drawString(hLayerChar, 220, 130, 2);

  tft.drawString("EXP. TIME:", 20, 160, 2);
  String expotimeString = String(expotime);
  char expotimeChar[7];
  expotimeString.toCharArray(expotimeChar, 7);
  tft.drawString(expotimeChar, 220, 160, 2);

  tft.drawString("INITIAL EXP.:", 20, 190, 2);
  String iexpotimeString = String(iexpotime);
  char iexpotimeChar[7];
  iexpotimeString.toCharArray(iexpotimeChar, 7);
  tft.drawString(iexpotimeChar, 220, 190, 2);

}


void screen16() {

  long int timelayersinitial = FirstLayers * iexpotime;
  long int quantitylayersresto = Layers - FirstLayers;
  long int timerestodelayers = quantitylayersresto * expotime;
  long int timesubebajatot = Layers * updowntime;
  long int timetotalseg = timesubebajatot + timelayersinitial + timerestodelayers;
  long int timetotalmin = timetotalseg / 60;
  int timetotalhours = timetotalmin / 60;
  int restominutes = timetotalmin - timetotalhours * 60;

  tft.setRotation(3);
  blackscreen();
  tft.setTextColor(ILI9341_WHITE);
  bannerprint();

  tft.drawCentreString("DETAILS", 160, 55, 2);

  tft.drawString("TOTAL LAYERS:", 14, 100, 2);
  String LayersString = String(Layers);
  char LayersChar[7];
  LayersString.toCharArray(LayersChar, 7);
  tft.drawString(LayersChar, 226, 100, 2);

  tft.drawString("DURATION:", 14, 135, 2);
  String timetotalhoursString = String(timetotalhours);
  char timetotalhoursChar[7];
  timetotalhoursString.toCharArray(timetotalhoursChar, 7);
  tft.drawCentreString(timetotalhoursChar, 175, 135, 2);
  tft.drawString("h", 195, 135, 2);
  String restominutesString = String(restominutes);
  char restominutesChar[7];
  restominutesString.toCharArray(restominutesChar, 7);
  tft.drawCentreString(restominutesChar, 248, 135, 2);
  tft.drawString("min", 272, 135, 2);

  tft.drawCentreString("Insert tray and start!", 160, 190, 2);

}


void screen17() {

  tft.setRotation(3);
  blackscreen();

  int heightup = Layers * hLayer;
  int heightremain = maxheight - heightup;

  movasc(heightremain, HighSpeed);

  while (digitalRead (BtnCancel) != LOW) {  //This is for led 13 to blink
    blackscreen();
    delay(100);
  }

}

////////////////////////////////////////////////////////////

// SCREEN SUPPORT FUNCTIONS



void arrows() {

  tft.fillTriangle(260, 135, 280, 135, 270, 120, ILI9341_WHITE );  //up arrow
  tft.fillTriangle(260, 165, 280, 165, 270, 180, ILI9341_WHITE );  //down arrow
}


void rectscreen() {

  tft.drawRect(120, 110, 80, 40, ILI9341_WHITE);
}


void blackscreen() {

  tft.fillScreen(ILI9341_BLACK);
}


void cleanscreen() {
  tft.fillRect(0, 40, 320, 200, ILI9341_BLACK);
}



void rectblue () {

  tft.fillRect(0, 0, 320, 40, ILI9341_BLUE);
}



void bannerprint() {

  rectblue();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("PRINT", 160, 6, 2);

}


void bannerpreparation() {

  rectblue ();
  tft.setTextColor(ILI9341_WHITE);
  tft.drawCentreString("Lite3DP.com", 160, 6, 2);

}




/***************************************************************************************
** Function name:           drawBMP
** Descriptions:            draw a BMP format bitmap to the screen
***************************************************************************************/

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size makes loading a little faster but the law of
// rapidly diminishing speed improvements applies.
// Suggest 8 minimum and 85 maximum (3 x this value is
// stored in a byte = 255/3 max!)
// A value of 8 is only ~20% slower than 24 or 48!
// Note that 5 x this value of RAM bytes will be needed
// Increasing beyond 48 gives little benefit.
// Use integral division of TFT (or typical often used image)
// width for slightly better speed to avoid short buffer purging

#define BUFF_SIZE 80

void drawBMP(char *filename, int x, int y, boolean flip) {
  if ((x >= tft.width()) || (y >= tft.height())) return;
  File     bmpFile;
  int16_t  bmpWidth, bmpHeight;   // Image W+H in pixels
  //uint8_t  bmpDepth;            // Bit depth (must be 24) but we dont use this
  uint32_t bmpImageoffset;        // Start address of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFF_SIZE];    // SD read pixel buffer (8 bits each R+G+B per pixel)
  uint16_t tftbuffer[BUFF_SIZE];       // TFT pixel out buffer (16-bit per pixel)
  uint8_t  sd_ptr = sizeof(sdbuffer); // sdbuffer pointer (so BUFF_SIZE must be less than 86)
  boolean  goodBmp = false;            // Flag set to true on valid header parse
  int16_t  w, h, row, col;             // to store width, height, row and column
  //uint8_t  r, g, b;   // brg encoding line concatenated for speed so not used
  uint8_t rotation;     // to restore rotation
  uint8_t  tft_ptr = 0;  // buffer pointer

  // Check file exists and open it
  if ((bmpFile = SD.open(filename)) == NULL) {
    //  Serial.println(F("File not found")); // Can comment out if not needed
    return;
  }


  // Parse BMP header to get the information we need
  if (read16(bmpFile) == 0x4D42) { // BMP file start signature check
    read32(bmpFile);       // Dummy read to throw away and move on
    read32(bmpFile);       // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    read32(bmpFile);       // Dummy read to throw away and move on
    bmpWidth  = read32(bmpFile);  // Image width
    bmpHeight = read32(bmpFile);  // Image height

    //if (read16(bmpFile) == 1) { // Number of image planes -- must be '1'
    // Only proceed if we pass a bitmap file check
    if ((read16(bmpFile) == 1) && (read16(bmpFile) == 24) && (read32(bmpFile) == 0)) { // Must be depth 24 and 0 (uncompressed format)
      //goodBmp = true; // Supported BMP format -- proceed!
      // BMP rows are padded (if needed) to 4-byte boundary
      rowSize = (bmpWidth * 3 + 3) & ~3;
      // Crop area to be loaded
      w = bmpWidth;
      h = bmpHeight;

      // We might need to alter rotation to avoid tedious pointer manipulation
      // Save the current value so we can restore it later
      rotation = tft.getRotation();
      // Use TFT SGRAM coord rotation if flip is set for 25% faster rendering
      if (flip) tft.setRotation((rotation + (flip << 2)) % 8); // Value 0-3 mapped to 4-7

      // We might need to flip and calculate new y plot coordinate
      // relative to top left corner as well...
      switch (rotation) {
        case 0:
          if (flip) y = tft.height() - y - h; break;
        case 1:
          y = tft.height() - y - h; break;
          break;
        case 2:
          if (flip) y = tft.height() - y - h; break;
          break;
        case 3:
          y = tft.height() - y - h; break;
          break;
      }

      // Set TFT address window to image bounds
      // Currently, image will not draw or will be corrputed if it does not fit
      // TODO -> efficient clipping, I don't need it to be idiot proof ;-)
      tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

      // Finally we are ready to send rows of pixels, writing like this avoids slow 32 bit multiply
      for (uint32_t pos = bmpImageoffset; pos < bmpImageoffset + h * rowSize ; pos += rowSize) {
        // Seek if we need to on boundaries and arrange to dump buffer and start again
        if (bmpFile.position() != pos) {
          bmpFile.seek(pos);
          sd_ptr = sizeof(sdbuffer);
        }

        // Fill the pixel buffer and plot
        for (col = 0; col < w; col++) { // For each column...
          // Time to read more pixel data?
          if (sd_ptr >= sizeof(sdbuffer)) {
            // Push tft buffer to the display
            if (tft_ptr) {
              // Here we are sending a uint16_t array to the function
              tft.pushColors(tftbuffer, tft_ptr);
              tft_ptr = 0; // tft_ptr and sd_ptr are not always in sync...
            }
            // Finally reading bytes from SD Card
            bmpFile.read(sdbuffer, sizeof(sdbuffer));
            sd_ptr = 0; // Set buffer index to start
          }
          // Convert pixel from BMP 8+8+8 format to TFT compatible 16 bit word
          // Blue 5 bits, green 6 bits and red 5 bits (16 bits total)
          // Is is a long line but it is faster than calling a library fn for this
          tftbuffer[tft_ptr++] = (sdbuffer[sd_ptr++] >> 3) | ((sdbuffer[sd_ptr++] & 0xFC) << 3) | ((sdbuffer[sd_ptr++] & 0xF8) << 8);
        } // Next row
      }   // All rows done

      // Write any partially full buffer to TFT
      if (tft_ptr) tft.pushColors(tftbuffer, tft_ptr);
    } // End of bitmap access
  }   // End of bitmap file check
  //}     // We can close the file now

  bmpFile.close();
  //if(!goodBmp) Serial.println(F("BMP format not recognized."));
  tft.setRotation(rotation); // Put back original rotation
}

/***************************************************************************************
** Function name:           Support functions for drawBMP()
** Descriptions:            Read 16- and 32-bit types from the SD card file
***************************************************************************************/

// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File & f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File & f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
