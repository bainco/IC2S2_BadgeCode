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

datetime dt;                                  // Date/teme values
int MY_TIME;                                       // Epoch time value
char dates[9];                                // Date string
char times[9];                                // Time string

unsigned short theirID;
unsigned short theirAnswers[NUM_QUESTIONS];

unsigned int serverTime;

int LISTENFORSERVER_TIMEOUT = 100;
void sendBeacon();
void listenForServer();

void storeServerCount(short count);
void storeUserCount(short count);
short retrieveCount(); // Actually returns the count. No referencing.
short retrieveUserCount(); // Actually returns the count. No referencing.
short retrieveServerCount(); // Actually returns the count. No referencing.


int storeContact(unsigned int timeVal, short idVal);
int retrieveContact(int addr, unsigned int *timeVal, short *idVal);

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
        screen_scrollStop();
        P17_Check_In();
        break;

      case 0b0000100:                          // P25 Pressed - upload all contacts
        screen_scrollStop();
        P25_Upload_Contacts();
        break;

      case 0b0000001:
        screen_scrollStop();
        P27_PartToPart();
        break;

      case 0b1111111:                         // OSH pressed, erase EEPROM
        screen_scrollStop();
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
   irclear();
   listenForServer();
   rgbs(OFF, OFF);
   Display_Main_Menu();
}

void P27_PartToPart() {
   clear();

   char received = 0;
   int irlenb = 0;

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

void listenForServer() {
   int i = 0;
   while(i < LISTENFORSERVER_TIMEOUT) {
    rgbs(GREEN, GREEN);
    memset(&theirID, 0, sizeof(theirID));        // Clear their variables
    memset(&serverTime, 0, sizeof(serverTime));

    irscan("%d%d", &serverTime, &theirID);

    if(theirID > 0) {
       rgbs(CYAN, CYAN);
       print("Heard from... id: %d ,", theirID);
       print("time: %d\n", serverTime);
       
       dt_set(dt_fromEt(serverTime));

       clear();
       oledprint("Check-in at %d");
       storeContact(serverTime, theirID);

       sendBeacon();
       pause(500);
       Display_Main_Menu();
       break;
    }
    pause(50);
    rgbs(OFF, OFF);

    i += 1;
  }
  rgbs(OFF, OFF);
}

void sendBeacon() {
  for(int i = 0; i < 5; i++) {
    rgbs(RED, RED);
    irprint("%d\n%16s\n", MY_ID, MY_FIRST_NAME);
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
             //S              E
   //oledprint("%s", dates);               // Something to shift
   //screen_scrollRight(0, 15);                 // Scroll left-right

}

void Display_Private_SumStats(unsigned int *y) {
   clear();
   text_size(SMALL);
   screen_auto(OFF);
            //S              ES              E
   oledprint("You've checked  in at %d places.\n\n", retrieveServerCount());
   oledprint("You've met\n %d people!\n", retrieveServerCount());

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

/**** EEPROM METHODS ****/
void storeUserCount(short count) {
  ee_writeShort(count, USER_COUNT_ADDRESS);
}

void storeServerCount(short count) {
  ee_writeShort(count, SERVER_COUNT_ADDRESS);
}

// RETURNS NUMBER OF USER INTERACTIONS
short retrieveUserCount() {
   if (ee_readShort(USER_COUNT_ADDRESS) == -1) {
    storeServerCount(0);
  }

  return ee_readShort(USER_COUNT_ADDRESS);
}


// RETURNS NUMBER OF SERVER INTERACTIONS
short retrieveServerCount() {
  if (ee_readShort(SERVER_COUNT_ADDRESS) == -1) {
    storeServerCount(0);
  }

  return ee_readShort(SERVER_COUNT_ADDRESS);
}


// RETURNS OVERALL COUNT
short retrieveCount() {

  if (ee_readShort(USER_COUNT_ADDRESS) == -1) {
    storeUserCount(0);
  }

  return retrieveServerCount() + retrieveUserCount();
}

int storeContact(unsigned int timeVal, short idVal) {

  unsigned int addr = MEM_START_ADDRESS + (retrieveCount() * 6);

  if(addr < (65536 - 6)) {
    ee_writeInt(timeVal, addr);
    addr += 4;
    ee_writeShort(idVal, addr);
    addr += 2;
    //int memRemaining = 65535 - (addr);
    //memRemaining /= 6;

    // CASES
    if (idVal < 1000) {
     // STAFF BADGE
     // STUB
    }
    else if (idVal < 2000) {
    // USER BADGE
      storeUserCount(retrieveUserCount() + 1);
    }
    else if (idVal < 3000) {
    // SERVER BADGE
      storeServerCount(retrieveServerCount() + 1);
    }
  }
  else {
    clear();
    oledprint("ERROR!\n");
  }
  return addr;
}

int retrieveContact(int addr, unsigned int *timeVal, short *idVal) {
  if(addr < (65536 - 6)) {
    *timeVal = ee_readInt(addr);
    addr += 4;
    *idVal = ee_readShort(addr);
    addr += 2;
  }
  else {
    clear();
    oledprint("ERROR!\n");
  }
  return addr;
}
