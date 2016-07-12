/**
RescueWizard.c
@Author Connor Bain

This is a non-functioning badge program designed to resuce information from
the EEPROM of badges that aren't working for various reasons.
*/

#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library
#include "eeprom.h"                          // Include EEPROM library
#include "survey.h"
#include "part_strings.h"                    // Include Part Info

/*** GLOBALS ***/
int *cog = 0;

// Display Methods
void Display_Main_Menu();
void myScroll();
void Display_Private_SumStats(unsigned int *y);

void print_all_contacts();
void P25_Upload_To_Computer();
/** END FUNCTION DEFS **/

/****** MAIN LOOP *****/
void main() {
  badge_setup();                             // Call badge setup
  text_size(SMALL);
  oledprint("Expecting %d\n", retrieveCount());
  oledprint("Please wait...\n");
  pause(10000);

  clear();
  oledprint("Uploading...\n");
  pause(1000);

  oledprint("Send 1.\n");
  P25_Upload_To_Computer();
  pause(1000);

  oledprint("Send 2.\n");
  P25_Upload_To_Computer();
  pause(1000);

  oledprint("Send 3.\n");
  P25_Upload_To_Computer();
  pause(1000);

  clear();

  int states;                                  // For all button states
  while(1)                                   // States loop
  {
    states = buttons();                      // Load buttons output to states
    leds(states);                            // Light up corresponding LEDs

    cursor(0, 0);                            // Position cursor
    oledprint("%07b", states);               // Display states as binary number
    if(states == 0b1111111) break;           // If OSH pressed, exit loop
  }
  Display_Main_Menu();
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


// Method to upload to computer on P25 pressed
void P25_Upload_To_Computer() {
   print_all_contacts();
}

// Method to print all contacts to serial port
void print_all_contacts() {
   int t;
   int id;
   unsigned int address = MEM_START_ADDRESS;

   short currentCount = retrieveCount(); // Get the current count
   print("MY_ID\n");
   print("%d\n", MY_ID);
   print("MY_DATA\n");
   for(int i = 0; i < currentCount; i++) {
      address = retrieveContact(address, &t, &id);
      print("%d,%d\n", t, id);
   }
}
