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
#include "sound.h"         // Include sound library
#include "datetime.h"      // Include datetime library
#include "ID_Info.h"       // Include ID Info

/***** UNIQUE GLOBALS *****/
short MY_LOCATION_ID = 2001;     // SERVER ID
char MY_NAME[32] = "KEYNOTE";    // SERVER NAME (7 chars wide)
/*** END UNIQUE GLOBALS ***/

/***** GLOBAL VARIABLES *****/
sound *audio;  // Pointer for audio process

// Time Variables
datetime dt = {2016, 5, 10, 5, 5, 00}; // Date time object
int et;                                // Epoch time

// Comm Variables
short recentID = 0;
short theirID;
char theirName[32];
int theirTime;

// Button Variables
int states;
/***** END GLOBALS *****/

/***** FUNCTION DEFS *****/
void sendBeacon( int et);
void listen( int et);
/*** END FUNCTION DEFS ***/

/****** MAIN LOOP *****/
void main() {
   badge_setup();       // setup
   dt_run(dt);          // Use to start system timer

   audio = sound_run(9, 10);      // Run sound, get audio pointer
   sound_volume(audio, 0, 100);   // Set channel volumes

   Display_Main_Menu();
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
  //  irclear();
    sendBeacon(et);
    listen(et);
    pause(50);
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
   pause(500);

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
   // Clear "their" variables
   memset(&theirID, 0, sizeof(theirID));
   memset(&theirName, 0, sizeof(theirName));
   memset(&theirTime, 0, sizeof(theirTime));

   // Check IR buffer for messages
   irscan("%d, %d, %32s", &theirID, &theirTime, &theirName);
   if(strlen(theirName) > 0) {
      //print("Heard from... id: %d ", theirID);
      //print("name: %s\n", theirName);
      //print("Time: %d\n", theirTime);

      // It might just be the TimeWizard. If so, update time.
      if (theirID == TIME_WIZARD_ID) {
         clear();
         oledprint("UPDATE\nTIME");
         dt_set(dt_fromEt(theirTime));
      }

      // Otherwise, if we haven't just heard from them, record the interaction.
      else if(theirID != recentID) {
         recentID = theirID;        // Update recentID
         storeContact(et, theirID); // Store the contact in EEPROM

         //print("Saving... timestamp: %d, id: %d\n", et, theirID);

         // Welcome the user (audio too)
         rgbs(CYAN, CYAN);
         clear();
         oledprint("Welcome  %s", theirName);
         oledprint("ID: %d", theirID);
         sound_note(audio, 0, E5);                    // Play first note with ch 0
         pause(500);
         sound_note(audio, 0, C5);                    // Play first note with ch 0
         pause(500);
         sound_note(audio, 0, 0);                    // Play first note with ch 0
      }
      // Otherwise, we just saw them. Welcome, but not too friendly
      else {
         rgbs(YELLOW, YELLOW);
         clear();
         oledprint("Hi again\n%s", theirName);
         sound_note(audio, 0, E5);                    // Play first note with ch 0
         pause(500);
         sound_note(audio, 0, 0);                    // Play first note with ch 0
      }
      // Pause and go back to display
      pause(1000);
      Display_Main_Menu();
   }
}

// Method to send the server beacon
void sendBeacon(int et) {
   clear();
   //text_size(SMALL);
   // oledprint("Time: %d\n", et);
   // oledprint("ID: %d", MY_LOCATION_ID);
   // rgbs(RED, RED);                          // Signal transmitting
   irprint("%d, %d, %32s", et, MY_LOCATION_ID, MY_NAME);
   //irprint("%d, %d", et, MY_LOCATION_ID); // Transmit epoch time
   rgbs(OFF, OFF);                          // Finish transmitting
}

// Method to print all contacts to serial port
// THIS NEEDS TO BE UPDATED
// I'm THINKING JUST TURN THIS INTO THE BOOZE WIZARD CODE FROM PART
void print_all_contacts() {
   int t;
   short id;
   unsigned int address = MEM_START_ADDRESS;

   short currentCount = retrieveCount(); // Get the current count
   print("Current Contact Count: %d\n", currentCount);

   for(int i = 0; i < currentCount; i++) {
      address = retrieveContact(address, &t, &id);
      print("%d, %d\n", t, id);
   }
}

/***** Display Methods *****/
void Display_Main_Menu() {
   clear();
   text_size(LARGE);
   oledprint("%s\nCheck-in", MY_NAME);
}
/**** END DISPLAY METHODS ****/
