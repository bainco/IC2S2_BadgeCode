/**
server-code.c
@Author Connor Bain

This class contains all of the main code for the server IC2S2 electronic conference
badges. It serves as the template for all server badges and includes
the main menu, check-in comms, and saving data to EEPROM.
*/
#include "simpletools.h"   // Include simpletools library
#include "badgetools.h"    // Include badgetools library
#include "eeprom.h"        // Include EEPROM library
#include "datetime.h"      // Include datetime library
#include "ID_Info.h"       // Include ID wInfo

/***** UNIQUE GLOBALS *****/
int MY_LOCATION_ID = 2044;     // SERVER ID
char MY_NAME[32] = "J 101";    // SERVER NAME (7 chars wide)
/*** END UNIQUE GLOBALS ***/

/***** GLOBAL VARIABLES *****/
// Time Variables
datetime dt = {2016, 6, 20, 22, 27, 00}; // Date time object
int et;                                // Epoch time

// Comm Variables
int recentID = 0;
int theirID;
char theirName[32];
int theirTime;

// Button Variables
int states;
/***** END GLOBALS *****/

/***** FUNCTION DEFS *****/
void sendBeacon( int et);
void listen( int et);
void Display_Main_Menu();
void All_OSH_Erase_EEPROM();
void P25_Upload_To_Computer();
void print_all_contacts();
/*** END FUNCTION DEFS ***/

/****** MAIN LOOP *****/
void main() {
   badge_setup();       // setup
   dt_run(dt);          // Use to start system timer

   Display_Main_Menu();
   irclear();
   
   pause(10000);
  P25_Upload_To_Computer();
    P25_Upload_To_Computer();

  P25_Upload_To_Computer();


   while (1) {
      states = buttons();
      switch(states) {                         // Handle button press
         case 0b0000100:                         // P25 Pressed - upload all contacts
            P25_Upload_To_Computer();
            break;                             // Back to while(1)
         case 0b1111111:                         // OSH pressed, erase EEPROM
            All_OSH_Erase_EEPROM();
         default:
            break;                               // Continue through while(1)
      }

    dt = dt_get();
    et = dt_toEt(dt); // Convert from date time to epoch time
    sendBeacon(et);
    listen(et);
    pause(100);
  }
}
/****** END MAIN LOOP *****/

/***** BUTTON FUNCTIONS *****/
// Method to upload to computer on P25 pressed
void P25_Upload_To_Computer() {
   clear();
   text_size(SMALL);
   oledprint("Upload  to\ncomputer");
   print_all_contacts();
   oledprint("Done");
   pause(250);
   Display_Main_Menu();
}

// Method to erase EEPROM.
// You need to hold down 7 butons, so this should never happen.
void All_OSH_Erase_EEPROM() {
   clear();
   text_size(LARGE);
   oledprint("CONFIRM");
   pause(1000);

   states = buttons();

   text_size(SMALL);
   cursor(0, 4);
   oledprint("Yes-OSH\n\n");
   oledprint("No-Any other btn");

   while(!(states = buttons()));         // Get confirmation

   // If button is OSH, Erase!
   if(states == 0b1000000) {
      clear();
      text_size(LARGE);
      oledprint("Wait 20s.");
      rgbs(YELLOW, YELLOW);
      contacts_eraseAll();
      rgbs(OFF, OFF);
      oledprint("Done!");
      pause(250);
      Display_Main_Menu();
   }
   // Otherwise, exit
   else {
     pause(250);
     Display_Main_Menu();
  }
}
/***** END BUTTON FUNCTIONS *****/

/***** HELPER FUNCTIONS *****/
void listen(int et) {

   char trimName[7] = "";

   int irlenb = 0;
   // Clear "their" variables
   memset(&theirID, 0, sizeof(theirID));
   memset(&theirName, 0, sizeof(theirName));
   memset(&theirTime, 0, sizeof(theirTime));
   pause(500);
   // Check IR buffer for messages
   irlenb = irscan("%d,%d,%32s", &theirID, &theirTime, &theirName);
   if(irlenb > 0) {
      // It might just be the TimeWizard. If so, update time.
      if (theirID == TIME_WIZARD_ID) {
         clear();
         oledprint("UPDATE\nTIME");
         dt_set(dt_fromEt(theirTime));
         recentID = theirID;
      }

      // Otherwise, if we haven't just heard from them, record the interaction.
      else if(theirID != recentID) {
         recentID = theirID;        // Update recentID
         storeContact(et, (short) theirID); // Store the contact in EEPROM

         for (char i = 0; i < 7; i++) {
            trimName[i] = theirName[i];
         }
         // Welcome the user (audio too)
         rgbs(CYAN, CYAN);
         clear();
         text_size(LARGE);
         cursor(0, 0);
         oledprint("Welcome\n");
         cursor(0, 1);
         oledprint("%7s", trimName);
      }
      // Otherwise, we just saw them. Welcome, but not too friendly
      else {
         for (char i = 0; i < 7; i++) {
            trimName[i] = theirName[i];
         }
         rgbs(YELLOW, YELLOW);
         clear();
         text_size(LARGE);
         cursor(0,0);
         oledprint("Welcome\n");
         cursor(0, 1);
         oledprint("%7s", trimName);
      }
      // Pause and go back to display
      pause(1000);
      Display_Main_Menu();
      irclear();
   }
}

// Method to send the server beacon
void sendBeacon(int et) {                      // Signal transmitting
   irprint("%d,%d,%32s", et, MY_LOCATION_ID, MY_NAME);
   rgbs(OFF, OFF);                          // Finish transmitting
}

// Method to print all contacts to serial port
// THIS NEEDS TO BE UPDATED
// I'm THINKING JUST TURN THIS INTO THE BOOZE WIZARD CODE FROM PART
void print_all_contacts() {
   int t;
   int id;
   unsigned int address = MEM_START_ADDRESS;

   short currentCount = retrieveCount(); // Get the current count
   print("Current Contact Count: %d\n", currentCount);

   for(int i = 0; i < currentCount; i++) {
      address = retrieveContact(address, &t, &id);
      print("%d,%d\n", t, id);
   }
}

/***** Display Methods *****/
void Display_Main_Menu() {
   clear();
   text_size(LARGE);
   oledprint("%s\nCheck-in", MY_NAME);
}
/**** END DISPLAY METHODS ****/
