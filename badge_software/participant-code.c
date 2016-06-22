/**
participant-code.c
@Author Connor Bain

This class contains all of the main code for the IC2S2 electronic conference
badges. It serves as the template for all conference attendee badges and includes
the main menu, check-in, part-to-part comms, and saving data to EEPROM.
*/

#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library
#include "datetime.h"                        // Include datetime library
#include "eeprom.h"                          // Include EEPROM library
#include "survey.h"                          // Include Survey library
#include "ID_Info.h"                         // Include ID info
#include "part_strings.h"                    // Include Part Info

/*** GLOBALS ***/
short LISTENFORSERVER_TIMEOUT = 100;

int *cog = 0;

// Variables for Time
datetime dt;                                  // Date/teme values
int MY_TIME;                                  // Epoch time value
char dates[9];                                // Date string
char times[9];                                // Time string

// Variables for Comms
int serverTime;
int serverID;

int theirID;
int theirAnswers[NUM_QUESTIONS];
char theirName[32];
/** END GLOBALS **/

/** FUNCTION DEFINITIONS **/
// Comm Methods
void sendBeacon();
void listenForServer();

// Button Methods
void All_OSH_Erase_EEPROM();
void P17_Check_In();
void P27_PartToPart();

// Display Methods
void Display_Main_Menu();
void myScroll();
void Display_Private_SumStats(unsigned int *y);
/** END FUNCTION DEFS **/

/****** MAIN LOOP *****/
void main() {
  badge_setup();                             // Call badge setup

  MY_TIME = ee_readInt(TIME_ADDRESS);        // Get most recent time

  if (MY_TIME == -1) {
      dt = dt_fromEt(MY_INIT_TIME);                         // Convert from epoch to datetime
      ee_writeInt(dt_toEt(dt), TIME_ADDRESS);
  }

  else {
      dt = dt_fromEt(MY_TIME);
  }

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
      case 0b0100000:
        if (cog != 0) {
         cog_end(cog);
         cog = 0;
        }
        screen_auto(ON);
        P17_Check_In();                       // P17 pressed, Server comms
        break;

      case 0b0000001:                         // P27 pressed, Part Comms
        if (cog != 0) {
         cog_end(cog);
         cog = 0;
        }
        screen_auto(ON);
        P27_PartToPart();
        break;

      case 0b1111111:                         // OSH pressed, erase EEPROM
        if (cog != 0) {
         cog_end(cog);
         cog = 0;
        }          screen_auto(ON);
        All_OSH_Erase_EEPROM();
        break;

       default:                               // otherwise, check for private mode
         y = accel(AY);
          if (y < -40) {
            if (cog != 0) {
              cog_end(cog);
              cog = 0;
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

/***** BUTTON METHODS ******/
void P17_Check_In() {
   clear();
   oledprint("Check-in Mode");
   irclear();
   listenForServer();
   rgbs(OFF, OFF);
   Display_Main_Menu();
}

void P27_PartToPart() {

   // Prep to receive
   char received = 0;
   int irlenb = 0;
   memset(&theirID, 0, sizeof(theirID));
   memset(&theirAnswers, 0, sizeof(theirAnswers));
   irclear();

   // Display to User
   clear();
   text_size(SMALL);
   oledprint("Hold still\nplease!");

   // Send info via IR
   rgbs(GREEN, GREEN);
   pause(1000);
   rgbs(RED, RED);
   clear();
   for (int i = 0; i < 2; i++) {
      oledprint("%d,%d,%d,%d,%d\n", MY_ID, MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
      irprint("%d,%d,%d,%d,%d", MY_ID, MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
      pause(20);
   }
   rgbs(OFF, OFF);
   pause(1000);

   // Look into IR Buffer for Response
   irlenb = irscan("%d,%d,%d,%d,%d", &theirID, &theirAnswers[0], &theirAnswers[1], &theirAnswers[2], &theirAnswers[3]);

 rgbs(RED, RED);
   clear();
   for (int i = 0; i < 2; i++) {
      oledprint("%d,%d,%d,%d,%d\n", MY_ID, MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
      irprint("%d,%d,%d,%d,%d", MY_ID, MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
      pause(20);
   }
   rgbs(OFF, OFF);

   if (irlenb > 0) {
      rgbs(CYAN, CYAN);
      received = 1;
      pause(200);
      rgbs(OFF, OFF);
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
   }
   // Otherwise, no message received
   else {
      clear();
      oledprint("No message      received.\n");
      oledprint("Please try again");
      pause(2000);
   }
   irclear();
   Display_Main_Menu();
}

// Method to erase EEPROM.
// You need to hold down 7 butons, so this should never happen.
void All_OSH_Erase_EEPROM() {
  clear();
  oledprint("CONFIRM");
  pause(1000);

  int states = buttons();

  text_size(SMALL);
  cursor(0, 4);
  oledprint("Yes-OSH\n\n");
  oledprint("No-Any other btn");
  while(!(states = buttons()));         // Get confirmation
  if(states == 0b1000000)               // Yes, erase
  {
     text_size(LARGE);
     clear();
     oledprint("Wait 20s...");
     contacts_eraseAll();
     oledprint("Done!");
     pause(250);
     Display_Main_Menu();
   }
  else                                   // No, don't erase
  {
     pause(250);
     Display_Main_Menu();
  }
}

/** HELPER METHODS **/

// Method to transmit all contacts via IR
void irTxAll() {
   // Comm variables
   int aTime;
   int anID;

   unsigned int address = MEM_START_ADDRESS;    // Get ready to read
   short currentCount = retrieveCount();        // Get the current count

   // For every contact
   for(int i = 0; i < currentCount; i++) {
      // Clear their variables
      memset(&aTime, 0, sizeof(aTime));
      memset(&anID, 0, sizeof(anID));

      address = retrieveContact(address, &aTime, &anID); // Grab contact from memory

      rgbs(RED, RED);
      clear();
      text_size(SMALL);
      oledprint("SENDING...%d\n", i);
      oledprint("%d\n%d\n", aTime, anID);
      irprint("%d,%d", aTime, anID);
      pause(500);
      rgbs(OFF, OFF);
   }
   // Send terminate command
   clear();
   text_size(SMALL);
   oledprint("TERMINATE.");
   irprint("%d,%d", (int) TERMINATE, (int) TERMINATE);
   pause(1000);
}

// Method to wait for a sever beacon until timeout
void listenForServer() {
   short i = 0;
   int irlenb = 0;
   while(i < LISTENFORSERVER_TIMEOUT) {
      rgbs(GREEN, GREEN);
      // Clear their variables
      memset(&serverID, 0, sizeof(serverID));
      memset(&serverTime, 0, sizeof(serverTime));
      memset(&theirName, 0, sizeof(theirName));

      irlenb = irscan("%d,%d,%32s", &serverTime, &serverID, &theirName);
      if(strlen(theirName) > 0) {
         irclear();
         rgbs(CYAN, CYAN);
         clear();
         text_size(SMALL);
         oledprint("ID: %d\n", serverID);
         oledprint("Time: %d\n", serverTime);
         oledprint("Name: %32s", theirName);
         pause(50);

         // Special case for BoozeWizard
         if (serverID == BOOZE_WIZARD_ID) {
            clear();
            text_size(LARGE);
            oledprint("HOLD\nSTILL");
            rgbs(RED, RED);
            irprint("%d,%d,%32s", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
            pause(1000);
            // Send beacon to BoozeWizard
            clear();
            irTxAll();  // Transmit all data
            rgbs(OFF, OFF);
            return;  // Done, leave the method
         }

         // Otherwise, either TimeWizard or Server
         dt_set(dt_fromEt(serverTime));   // Update time to server time

         // Store interaction, send server beacon
         if (serverID != TIME_WIZARD_ID) {
            clear();
            text_size(SMALL);
            oledprint("Check-in at\n%d", serverID);
            storeContact(serverTime, (short) serverID);
            sendBeacon();
         }
         // Otherwise, this is just a time update
         else {
            clear();
            text_size(SMALL);
            oledprint("Time Update!");
         }
         pause(500);
         rgbs(OFF, OFF);
         return;
      }
      pause(50);
      rgbs(OFF, OFF);
      i += 1; // Continue listen loop
   }
   // If we're here, we never got anything.
   rgbs(RED, RED);
   clear();
   text_size(SMALL);
   oledprint("No check-in.\nPlease try again");
   pause(1000);
   rgbs(OFF, OFF);
   return;
}

// Method to send an IR beacon to the server
void sendBeacon() {
   for(int i = 0; i < 3; i++) {
      rgbs(RED, RED);
      irprint("%d,%d,%32s", MY_ID, dt_toEt(dt), MY_FIRST_NAME);

      clear();
      text_size(SMALL);
      oledprint("%d\n%d\n%32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
      pause(500);
      rgbs(OFF, OFF);
   }
}

/**** DISPLAY METHODS ******/
void Display_Main_Menu() {
   clear();
   text_size(LARGE);

   if ((strlen(MY_FIRST_NAME) > 8) || (strlen(MY_LAST_NAME) > 8)) {
      cog = cog_run(&myScroll, 128);
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
   text_size(SMALL);
   screen_auto(OFF); // Disable update while we filp the screen
            //S              ES              E
   oledprint("You've checked  in at %d places.\n\n", retrieveServerCount());
   oledprint("You've met\n %d people!\n", retrieveUserCount());
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
