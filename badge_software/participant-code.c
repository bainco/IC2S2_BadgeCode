/**
Staff IDs are below 1000
Attendee IDS ARE IN THE 1000s
Server IDs are in the 2000s

Policy is to clear at the beginning of any display command.

Anything that just posts a screen should start with "Display"

Anything that is the direct result of a button press should start with that button (i.e. P17_Check_in)

Waits are used before a helper method
*/

#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library
#include "datetime.h"                         // Include datetime library
#include "eeprom.h"
#include "survey.h"
#include "part_strings.h"

/*** GLOBALS ***/
short TIME_WIZARD_ID = 9999;
short BOOZE_WIZARD_ID = 10000;

short TERMINATE = 13;

datetime dt;                                  // Date/teme values
int MY_TIME;                                       // Epoch time value
char dates[9];                                // Date string
char times[9];                                // Time string

short serverID;
short theirID;
short theirAnswers[NUM_QUESTIONS];
char theirName[32];

int serverTime;

short LISTENFORSERVER_TIMEOUT = 100;
void sendBeacon();
void listenForServer();

void print_all_contacts();

void P25_Upload_Contacts();
void All_OSH_Erase_EEPROM();
void P17_Check_In();
void P27_PartToPart();

void Display_Main_Menu();
void Display_Private_SumStats(unsigned int *y);

void main()                                  // Main function
{
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
  while(1)
  {
    dt = dt_get();                             // Get current system time
    MY_TIME = dt_toEt(dt);                        // Datetime -> epoch time
    dt_toDateStr(dt, dates);                   // Convert to strings
    dt_toTimeStr(dt, times);

    states = buttons();
    y = accel(AY);
    switch(states)
    {
      case 0b0100000:
        P17_Check_In();
        break;

      case 0b0000100:                          // P25 Pressed - upload all contacts
        P25_Upload_Contacts();
        break;

      case 0b0000001:
        P27_PartToPart();
        break;

      case 0b1111111:                         // OSH pressed, erase EEPROM
        All_OSH_Erase_EEPROM();
        break;

       default:
        if (y < -40) {
          screen_scrollStop();
          Display_Private_SumStats(&y);
        }
        if(dt.s == 0) {                             // If next minute over
           MY_TIME = dt_toEt(dt);                        // Datetime -> epoch time
           ee_writeInt(MY_TIME, TIME_ADDRESS);                  // Store in EEPROM
           led(0, ON);                              // Indicate storing P27 LED
           while(dt.s == 0) {                        // Wait for second 0 to finish                                        // to avoid multiple EE writes
             dt = dt_get();                         // Check time
         }
         led(0, OFF);                             // Second over, P27 LED off
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
   clear();

   char received = 0;
   int irlenb = 0;
   memset(&theirID, 0, sizeof(theirID));
   memset(&theirAnswers, 0, sizeof(theirAnswers));

   irclear();
      text_size(SMALL);

     oledprint("Hold still\nplease!");
     rgbs(GREEN, GREEN);
     pause(2000);
     rgbs(RED, RED);
     irprint("%d%c%c%c%c", MY_ID, MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
     rgbs(OFF, OFF);
     pause(500);

     irlenb = irscan("%d%c%c%c%c\n", &theirID, &theirAnswers[0], &theirAnswers[1], &theirAnswers[2], &theirAnswers[3]);
     if (irlenb > 0) {
        rgbs(YELLOW, YELLOW);
        received = 1;
        irclear();
        pause(500);
     }

     rgbs(OFF, OFF);

     if (received == 1) {

        // SAVE THE CONTACT
        dt = dt_get();                             // Get current system time
        MY_TIME = dt_toEt(dt);                        // Datetime -> epoch time
        storeContact(MY_TIME, theirID);

        // PICK RANDOM QUESTION
        short randIndex = rand() % NUM_QUESTIONS;

        if (MY_ANSWERS[randIndex] != theirAnswers[randIndex]) {
          for (short i = 0; i < NUM_QUESTIONS; i++) {
           if (MY_ANSWERS[i] ==  theirAnswers[i]) {
            randIndex = i;
            break;
           }
          }
        }

        clear();
        text_size(SMALL);

        // No matches!
        if (MY_ANSWERS[randIndex] != theirAnswers[randIndex]) {
                   //S              E
          oledprint("They answered   ");
        }
        else {     //S              E
          oledprint("Both answered   ");
        }

        //printf("MyTime: %d\n", MyTime);
        //printf("TheirID: %d\n", theirID);
        //printf("MyAnswers: %d %d %d %d\n", MY_ANSWERS[0], MY_ANSWERS[1], MY_ANSWERS[2], MY_ANSWERS[3]);
        //printf("TheirAnswers: %d %d %d %d\n", theirAnswers[0], theirAnswers[1], theirAnswers[2], theirAnswers[3]);
        //print("Selected: %d", randIndex);
        //print(SURVEY_Q_ANSWERS[randIndex][theirAnswers[randIndex]]);

        oledprint("%32s\n", SURVEY_Q_ANSWERS[randIndex][theirAnswers[randIndex]]);
        oledprint(" to:\n");
        oledprint(SURVEY_Q[randIndex]);
        rotate180();

        pause(10000);
     }
     pause(500);
     Display_Main_Menu();
}

void All_OSH_Erase_EEPROM() {
  clear();
  oledprint("CONFIRM");
  pause(500);

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

void P25_Upload_Contacts() {
   clear();
   text_size(SMALL);
   oledprint("Upload  to      computer");
   print_all_contacts();
   oledprint("Done");
   pause(250);
   clear();
   Display_Main_Menu();
}

/** HELPER METHODS **/
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

void irTxAll() {

   short currentCount = retrieveCount();

   int aTime;
   short anID;
   unsigned int address = MEM_START_ADDRESS;

   for(int i = 0; i < currentCount; i++)
   {
     memset(&aTime, 0, sizeof(aTime));        // Clear their variables
     memset(&anID, 0, sizeof(anID));
     address = retrieveContact(address, &aTime, &anID);
      clear();
      text_size(SMALL);
      oledprint("%d\n%d\n", aTime, anID);
      pause(1000);
     irprint("%d, %d\n", aTime, anID);
   }
   // SEND TERMINATE clear();
      text_size(SMALL);
      oledprint("%d\n%d\n", TERMINATE, TERMINATE);
      pause(1000);
   irprint("%d, %d\n", (int) TERMINATE, (short) TERMINATE);
}

void listenForServer() {
   int i = 0;
   int irlenb = 0;
   while(i < LISTENFORSERVER_TIMEOUT) {
    rgbs(GREEN, GREEN);
    memset(&serverID, 0, sizeof(serverID));        // Clear their variables
    memset(&serverTime, 0, sizeof(serverTime));
    memset(&theirName, 0, sizeof(theirName));

    irlenb = irscan("%d, %d, %32s", &serverTime, &serverID, &theirName);

    if(strlen(theirName) > 0) {
       rgbs(CYAN, CYAN);
   //    print("Heard from... id: %d ,", theirID);
//       print("time: %d\n", serverTime);
         clear();
         text_size(SMALL);
         oledprint("ID: %d\n", serverID);
         oledprint("Time: %d", serverTime);
         oledprint("Name: %32s", theirName);
         pause(1000);

      if (serverID == BOOZE_WIZARD_ID) {
         clear();
         rgbs(RED, RED);
         irprint("%d, %d, %32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
         irprint("%d, %d, %32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
         irprint("%d, %d, %32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
         irprint("%d, %d, %32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
         irprint("%d, %d, %32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
         pause(500);
         oledprint("HOLD\nSTILL");
         rgbs(RED, RED);
         irTxAll();
         clear();
         rgbs(OFF, OFF);
         return;
      }

       dt_set(dt_fromEt(serverTime));

       if (serverID != TIME_WIZARD_ID) {
          clear();
          text_size(SMALL);
          oledprint("Check-in at %d", serverID);
          storeContact(serverTime, serverID);
          sendBeacon();
       }
       else {
          clear();
          oledprint("Time Update!");
       }
       pause(500);
       Display_Main_Menu();
       return;
    }
    pause(50);
    rgbs(OFF, OFF);

    i += 1;
  }
  rgbs(OFF, OFF);
 return;
}

void sendBeacon() {
  for(int i = 0; i < 5; i++) {
    rgbs(RED, RED);
    irprint("%d, %d, %32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
    text_size(SMALL);
    clear();
    oledprint("%d\n%d\n%32s\n", MY_ID, dt_toEt(dt), MY_FIRST_NAME);
    pause(100);
    rgbs(OFF, OFF);
  }
}

/**** DISPLAY METHODS ******/
void Display_Main_Menu() {
   clear();
   text_size(LARGE);
   cursor(0, 0);
   oledprint("%s", MY_FIRST_NAME);
   cursor(0,1);
   oledprint("%s", MY_LAST_NAME);
   //oledprint("%s", dates);               // Something to shift
   //screen_scrollRight(0, 15);                 // Scroll left-right
}

void Display_Private_SumStats(unsigned int *y) {
   clear();
   text_size(SMALL);
   screen_auto(OFF);
            //S              ES              E
   oledprint("You've checked  in at %d places.\n\n", retrieveServerCount());
   oledprint("You've met\n %d people!\n", retrieveUserCount());

   rotate180();
   screen_update();

   while (*y < -40) {
    *y = accel(AY);
    pause(20);
   }

   clear();
   screen_auto(ON);                          // Enable screen auto-update
   Display_Main_Menu();
}
