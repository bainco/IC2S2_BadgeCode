#include "simpletools.h"
#include "badgetools.h"
#include "eeprom.h"
#include "sound.h"
#include "datetime.h"                         // Include datetime library

sound *audio;                                 // Pointer for audio process
datetime dt = {2016, 5, 10, 5, 5, 00};  // Set up the date/time
int et;

short TIME_WIZARD_ID = 9999;

short MY_LOCATION_ID = 2;
char MY_NAME[32] = "KEYNOTE";

short recentID = 0;

short theirID;
char theirName[32];
int theirTime;

int states;

void sendBeacon( int et);
void listen( int et);

void listen( int et) {

     memset(&theirID, 0, sizeof(theirID));        // Clear their variables
     memset(&theirName, 0, sizeof(theirName));        // Clear their variables
     memset(&theirTime, 0, sizeof(theirTime));        // Clear their variables

     //rgbs(GREEN, GREEN);                          // Signal transmitting
     irscan("%d, %d, %32s", &theirID, &theirTime, &theirName);

     if(strlen(theirName) > 0) {
       print("Heard from... id: %d ", theirID);
       print("name: %s\n", theirName);
       print("Time: %d\n", theirTime);

       if (theirID == TIME_WIZARD_ID) {
          clear();
          oledprint("UPDATE\nTIME");
          dt_set(dt_fromEt(theirTime));
       }

       else if(theirID != recentID) {
        recentID = theirID;

        storeContact(et, theirID);

        print("Saving... timestamp: %d, id: %d\n", et, theirID);

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
       else {
          rgbs(YELLOW, YELLOW);
          clear();
          oledprint("Hi again\n%s", theirName);
          sound_note(audio, 0, E5);                    // Play first note with ch 0
          pause(500);
          sound_note(audio, 0, 0);                    // Play first note with ch 0
       }
       pause(1000);
       Display_Main_Menu();
     }
     rgbs(OFF, OFF);                          // Signal transmittingå
}

void sendBeacon(int et) {

  int i = 10;
  clear();
  text_size(SMALL);
  oledprint("Time: %d\n", et);
  oledprint("ID: %d", MY_LOCATION_ID);
  pause(1000);
  // rgbs(RED, RED);                          // Signal transmitting
   irprint("%d, %d, %32s", et, MY_LOCATION_ID, MY_NAME);
   //irprint("%d, %d", et, MY_LOCATION_ID); // Transmit epoch time
   rgbs(OFF, OFF);                          // Finish transmitting
}

void main()
{
  badge_setup();
  dt_run(dt);

  audio = sound_run(9, 10);                   // Run sound, get audio pointer
  sound_volume(audio, 0, 100);                 // Set channel volumes

  rgbs(OFF, OFF);                                 // Use to start system timer

  Display_Main_Menu();
  while (1) {
    states = buttons();
    switch(states)                            // Handle button press
    {
      case 0b0000100:                         // P25 Pressed - upload all contacts
        clear();
        oledprint("Upload  to      computer");
        print_all_contacts();
        oledprint("Done");
        pause(250);
        Display_Main_Menu();
        continue;                             // Back to while(1)
      case 0b1111111:                         // OSH pressed, erase EEPROM
        clear();
        oledprint("CONFIRM");
        pause(500);
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
          Display_Main_Menu();
        }
        break;                               // Continue through while(1)
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

void Display_Main_Menu() {
   clear();
   oledprint("Keynote Check-in");
}

void print_all_contacts() {

  short currentCount = retrieveCount();

  print("Current Contact Count: %d\n", currentCount);

  unsigned int t;
  short id;
  int address = MEM_START_ADDRESS;

  for(int i = 0; i < currentCount; i++)
  {
    address = retrieveContact(address, &t, &id);
    print("%d, %d\n", t, id);
  }
}
