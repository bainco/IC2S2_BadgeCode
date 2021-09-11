/**
participant-code.c
@Author Connor Bain

This class contains all of the main code for the IC2S2 electronic conference
badges. It serves as the template for all wconference attendee badges and includes
the main menu, check-in, part-to-part comms, and saving data to EEPROM.
*/

#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library
#include "datetime.h"                        // Include datetime library
#include "eeprom.h"                          // Include EEPROM library
#include "survey.h"                          // Include Survey library
#include "ID_Info.h"                         // Include ID info

/****** EDITS PER PERSON HERE *******/
const int MY_ID = 997;
const char MY_FIRST_NAME[32] = "Connor  ";
const char MY_LAST_NAME[32] =  "Bain    ";
const int MY_ANSWERS[NUM_QUESTIONS] = {3,2,4,2};
/************/
/*** GLOBALS ***/
short LISTENFORSERVER_TIMEOUT = 100;
 
int *scrollCog = 0;
int *sendCog = 0;

// Variables for Time
datetime dt;                                  // Date/teme values
int MY_TIME;                                  // Epoch time value
char dates[9];                                // Date string
char times[9];                                // Time string

// Variables for Comms
int theirID;
int theirAnswers[NUM_QUESTIONS];

/** END GLOBALS **/

/** FUNCTION DEFINITIONS **/
// Comm Methods
void sendBeacon();

// Button Methods
void P27_PartToPart();

// Display Methods
void Display_Main_Menu();
void myScroll();
void Display_Private_SumStats(unsigned int *y);
/** END FUNCTION DEFS **/

/****** MAIN LOOP *****/
void main() {
  badge_setup();                             // Call badge setup
  srand(MY_ID);

  MY_TIME = ee_readInt(TIME_ADDRESS);        // Get most recent time

  dt_run(dt);                                 // Use to start system timer

  Display_Main_Menu();

  int states;
  int y;
  // Main loop for button pressing
  while(1) {
    // Update time
    dt = dt_get();                             // Get current system time
    MY_TIME = dt_toEt(dt);                     // Datetime -> epoch time
    dt_toDateStr(dt, dates);                   // Convert to strings
    dt_toTimeStr(dt, times);

    // BUTTON LOGIC
    states = buttons();
    switch(states) {
      case 0b0100001:                         // P17 and P27 pressed, Part Comms
        if (scrollCog != 0) {
         cog_end(scrollCog);
         scrollCog = 0;
        }
        screen_auto(ON);
        P27_PartToPart(&y);
        break;

      case 0b0001100:  
        if (scrollCog != 0) {
         cog_end(scrollCog);
         scrollCog = 0;
        }
        screen_auto(ON);
        BadgeSort_Process();        
        break;

       default:                               // otherwise, check for private mode
         y = accel(AY);
          if (y < -40) {
            if (scrollCog != 0) {
              cog_end(scrollCog);
              scrollCog = 0;
            }
            screen_auto(ON);
            Display_Private_SumStats(&y);
          }
        // Save time to EEPROM every minute
        if(dt.s == 0) {                             // If next minute over
           MY_TIME = dt_toEt(dt);                   // Datetime -> epoch time
           ee_writeInt(MY_TIME, TIME_ADDRESS);      // Store in EEPROM
           led(0, ON);                              // Indicate storing P27 LED
           while(dt.s == 0) {                       // Wait for second 0 to finish
             dt = dt_get();                         // to avoid multiple EE writes
         }
         led(0, OFF);                               // Second over, P27 LED off
      }
        break;
    }
  }
}


void BadgeSort_Process() {
  
  // Prep to receive
   char received = 0;
   int irlenb = 0;
   memset(&theirID, 0, sizeof(theirID));
   memset(&theirAnswers, 0, sizeof(theirAnswers));
   irclear();

   // Display to User
   clear();
   text_size(LARGE);
   oledprint("BADGE\nSORT");
   // Send info via IR
   sendCog = cog_run(&sendBeacon, 256);

   short j = 0;
   while (j < 20) {
      irlenb = irscan("%d,%d,%d,%d,%d", &theirID, &theirAnswers[0], &theirAnswers[1], &theirAnswers[2], &theirAnswers[3]);
      // Look into IR Buffer for Response
      if (irlenb > 0) {
         rgbs(CYAN, CYAN);
         received = 1;
         break;
      }
      pause((rand() % 100) + 300);
      j++;
   }
   
   if (received == 1) {   
     clear();    
     if (MY_ID > theirID) {
       oledprint("HIGH");
       rgbs(GREEN, GREEN);
     }
     
     else { 
       oledprint("LOW");
       rgbs(YELLOW, YELLOW);
     }   
   }     
 
   // Otherwise, no message received
   else {
      clear();
      oledprint("No message      received.\n");
      oledprint("Please try again");
   } 
  

   pause(4000);
   cog_end(sendCog);
   rgbs(OFF, OFF);
   
   Display_Main_Menu(); 
}  




void P27_PartToPart(unsigned int *y) {
   // Prep to receive
   char received = 0;
   int irlenb = 0;
   memset(&theirID, 0, sizeof(theirID));
   memset(&theirAnswers, 0, sizeof(theirAnswers));
   irclear();

   // Display to User
   clear();
   rgbs(GREEN, GREEN);
   text_size(SMALL);
   oledprint("Hold still\nplease!");
   // Send info via IR
   sendCog = cog_run(&sendBeacon, 256);

   short j = 0;
   while (j < 20) {
      irlenb = irscan("%d,%d,%d,%d,%d", &theirID, &theirAnswers[0], &theirAnswers[1], &theirAnswers[2], &theirAnswers[3]);
      // Look into IR Buffer for Response
      if (irlenb > 0) {
         rgbs(CYAN, CYAN);
         received = 1;
         break;
      }
      pause((rand() % 100) + 300);
      j++;
   }

   // If we got a message, do some processing
   if (received == 1) { 
      // SAVE THE CONTACT
      dt = dt_get();
      MY_TIME = dt_toEt(dt);
      storeContact(MY_TIME, (short) theirID);
      // Pick a random question
      // Either picks a similarity or worst-case dissim.
      short randIndex = rand() % NUM_QUESTIONS;
      if (MY_ANSWERS[randIndex] != theirAnswers[randIndex]) {
         for (short i = 0; i < NUM_QUESTIONS; i++) {
            if (MY_ANSWERS[i] ==  theirAnswers[i]) {
               randIndex = i;
               break;
            }
         }
      }
      
 
      // Display results
      clear();
      text_size(SMALL);
      // Display dissim
      if (MY_ANSWERS[randIndex] != theirAnswers[randIndex]) {
                   //S              E
         oledprint("They answered   ");
      }
      // Display sim
      else {     //S              E
         oledprint("Both answered   ");
      }

      oledprint("%32s\n", SURVEY_Q_ANSWERS[randIndex][theirAnswers[randIndex]]);
      oledprint(" to:\n");
      oledprint(SURVEY_Q[randIndex]);
      rotate180();   // Flip for private mode
      
      pause(10000);

      // While in private mode, keep doin
   while (*y < -40) {
    *y = accel(AY);
    pause(20);
   }
   
   }
   // Otherwise, no message received
   else {
      clear();
      oledprint("No message      received.\n");
      oledprint("Please try again");
      pause(2000);
   }
   
   irclear();
   cog_end(sendCog);
   rgbs(OFF, OFF);
   Display_Main_Menu();
}


/** HELPER METHODS **/

// Method to send an IR beacon to the server
void sendBeacon() {
    
   while(1) {
   for (char i = 0; i < 5; i++) {
     irprint("%d,%d,%d,%d,%d", MY_ID, MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
   }   
   pause((rand() % 100) + 50);
   }
}

/**** DISPLAY METHODS ******/
void Display_Main_Menu() {
   clear();
   text_size(LARGE);

   if ((strlen(MY_FIRST_NAME) > 8) || (strlen(MY_LAST_NAME) > 8)) {
      scrollCog = cog_run(&myScroll, 128);
   }

   else {
      cursor(0, 0);
      oledprint("%s", MY_FIRST_NAME);
      cursor(0, 1);
      oledprint("%s", MY_LAST_NAME);
   }
}

void myScroll() {
   screen_auto(OFF);
   char length = strlen(MY_FIRST_NAME);
   char position = 0;
   while (1) {
      for (char j = 0; j < 8; j++) {
         cursor(j, 0);
         oledprint("%c", MY_FIRST_NAME[(position + j) % length]);
         cursor(j, 1);
         oledprint("%c", MY_LAST_NAME[(position + j) % length]);
      }
      position = (position + 1) % length;
      screen_update();
      pause(25);
      if (position == 1) pause(5000);
   }
}

void Display_Private_SumStats(unsigned int *y) {
   clear();
   screen_auto(OFF); // Disable update while we filp the screen
   text_size(LARGE);
   oledprint("%d", MY_ID);
   rotate180();
   screen_update();  // UPDATE

   // While in private mode, keep doin
   while (*y < -40) {
    *y = accel(AY);
    pause(20);
   }
   // Exit
   clear();
   screen_auto(ON);  // Renable screen auto-update
   Display_Main_Menu();
}
