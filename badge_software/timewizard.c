#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library
#include "datetime.h"                         // Include datetime library
#include "eeprom.h"
#include "survey.h"

/*** GLOBALS ***/
short TIME_WIZARD_ID = 9999;

datetime dt = {2016, 6, 17, 5, 5, 00};                        // Convert from epoch to datetime
                           // Date/teme values
int MY_TIME;                                       // Epoch time value
char dates[9];                                // Date string
char times[9];                                // Time string

short theirID;
short serverID;
char serverName[32];
int serverTime;
int LISTENFORSERVER_TIMEOUT = 100;
void sendBeacon();
void listenForServer();

void All_OSH_Erase_EEPROM();
void P17_Check_In();
void P27_PartToPart();
void sendServerBeacon();

void sendServerBeacon() {

  // rgbs(RED, RED);                          // Signal transmitting
   irprint("%d, %d, %32s", dt_toEt(dt_get()), TIME_WIZARD_ID, "TIME WIZARD"); // Transmit epoch time
   rgbs(OFF, OFF);                          // Finish transmitting
}

void Display_Main_Menu();
void Display_Private_SumStats(unsigned int *y);

void P27_PartToPart() {
   short i = 0;
   while (i < 10) {
      irclear();
      sendServerBeacon();
      pause(50);
      i += 1;
   }
   Display_Main_Menu();
}

void main()                                  // Main function
{
  badge_setup();                             // Call badge setup

  MY_TIME = ee_readInt(TIME_ADDRESS);        // Get most recent time

  if (MY_TIME == -1) {
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
   }
  else                                   // No, don't erase
  {
  }
  pause(250);
  Display_Main_Menu();
}

void listenForServer() {
   int i = 0;
   int irlenb = 0;
   while(i < LISTENFORSERVER_TIMEOUT) {
    rgbs(GREEN, GREEN);
    memset(&theirID, 0, sizeof(serverID));        // Clear their variables
    memset(&serverTime, 0, sizeof(serverTime));
    memset(&serverName, 0, sizeof(serverName));


    irlenb = irscan("%d, %d, %32s", &serverTime, &serverID, &serverName);

    if(irlenb > 0) {
       rgbs(CYAN, CYAN);
       sendBeacon();
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
    irprint("%d, %d, %32s", TIME_WIZARD_ID, dt_toEt(dt_get()), "TIME WIZARD");
    clear();
    text_size(SMALL);
    oledprint("%d", dt_toEt(dt_get()));
    pause(1000);
    rgbs(OFF, OFF);
  }
}

/**** DISPLAY METHODS ******/
void Display_Main_Menu() {
   clear();
   text_size(LARGE);
   cursor(0, 0);
   oledprint("%d", TIME_WIZARD_ID);
   cursor(0,1);
   oledprint("BAIN");
}

void Display_Private_SumStats(unsigned int *y) {
   clear();
   text_size(SMALL);
   screen_auto(OFF);
            //S              ES              E
   oledprint("%s\n", dates);
   oledprint("%s", times);

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
