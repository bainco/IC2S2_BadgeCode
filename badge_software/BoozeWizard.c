#include "simpletools.h"
#include "badgetools.h"
#include "fdserial.h"
#include "eeprom.h"
#include "ID_Info.h"

char handshake[5];
fdserial* port;

unsigned int CLKLIMIT;

short theirID;
char theirName[32];
int theirTime;

short theID;
short anID;
int aTime;

void upload_contacts(fdserial* port);

void listen(fdserial* port);
void sendBeacon();

void listen_for_all();
void Display_Main_Menu();

void main() {

   badge_setup();
   simpleterm_close();
   oledprint("ERASING CONTACTS");
   contacts_eraseAll();
   CLKLIMIT = CLKFREQ * 2;

   // Connection to host routine (FORCE CONNECTION TO HOST)
   port = fdserial_open(31, 30, 0, 115200);
   text_size(SMALL);
   cursor(2, 4);
   oledprint("Connecting...");
   while(1) {
   dprint(port, "Propeller\n");
   pause(1000);  // We need this pause, since the host needs time to respond. 5 x 1 second = 10 second timeout
   if (fdserial_rxCount(port) == 0){
      continue;
   }
    else if (fdserial_rxCount(port) < 5)
    {
      fdserial_rxFlush(port);
      continue;
    }
    else dscan(port, "%s", handshake);
    // Attempt handshake and listen to response
    if (strcmp(handshake, "H0st") == 0)
    {
      break;
    }
  }

  Display_Main_Menu();

  irclear();
  while(1) {
    rgb(L, OFF);
    rgb(R, OFF);

    sendBeacon();
    listen(port);
    pause(50);
  }
}


/**

WHAT IS THIS?

if (CNT - t > CLKLIMIT)
{
clear();
text_size(LARGE);
cursor(0, 0);
oledprint(" ERROR");
text_size(SMALL);
cursor(0, 6);
oledprint("Please try again");
dprint(port, "txBegin\n");
dprint(port, "Timeout\n");
contacts_clearAll();
pause(2000);
clear();
break;
}





*/

void listen(fdserial* port) {
        memset(&theID, 0, sizeof(theID));        // Clear their variables
        memset(&theirName, 0, sizeof(theirName));        // Clear their variables
        memset(&theirTime, 0, sizeof(theirTime));        // Clear their variables

        //rgbs(GREEN, GREEN);                          // Signal transmitting
        irscan("%d, %d, %32s", &theID, &theirTime, &theirName);

        if(strlen(theirName) > 0) {
          //print("Heard from... id: %d ", theirID);
          //print("name: %s\n", theirName);
          //print("Time: %d\n", theirTime);
           rgbs(CYAN, CYAN);
           clear();
           oledprint("Welcome  %s", theirName);
           text_size(SMALL);
           pause(500);
           clear();
           oledprint("PLEASE HOLD STILL");
           // GO INTO FULL TIME LISTENING MODE
           listen_for_all();
           clear();
           oledprint("IR Upload Complete.");
           cursor(1, 0);
           oledprint("Serial Upload...");
           upload_contacts(port);
           oledprint("Process complete.");
           pause(100);
        }

        rgbs(OFF, OFF);                          // Signal transmittingå
}

void listen_for_all() {
   int irlenb = 0;
while(1) {
   memset(&aTime, 0, sizeof(aTime));        // Clear their variables
   memset(&anID, 0, sizeof(anID));
   irlenb = irscan("%d, %d\n", &aTime, &anID);

   if (irlenb > 0) {
     rgbs(YELLOW, YELLOW);

      clear();
      text_size(SMALL);
      oledprint("%d\n%d\n", aTime, anID);
      pause(100);

      if (aTime == TERMINATE && anID == TERMINATE) {
         irclear();
         return;
      }
      storeContact(aTime, anID);
      rgbs(OFF, OFF);
   }
}
}

void sendBeacon(int et) {
   irprint("%d, %d, %32s", et, BOOZE_WIZARD_ID, "BOOZE WIZARD");
   //irprint("%d, %d", et, MY_LOCATION_ID); // Transmit epoch time
   rgbs(OFF, OFF);                          // Finish transmitting
}

void Display_Main_Menu() {
   clear();
   text_size(LARGE);
   cursor(0, 0);
   oledprint("HOTSPOT!");
}

void upload_contacts(fdserial* port)
{
  char sendString[32];

  int c_count = retrieveCount();
  unsigned int address = MEM_START_ADDRESS;

  sprintf(sendString, "%d,%d\n", c_count, theID);

  oledprint("%32s\n", sendString);

  dprint(port, "txBegin\n");
  dprint(port, "%32s\n", sendString);

  for (int i = 0; i < c_count; i++)
  {
    memset(&theirTime, 0, sizeof(theirTime));        // Clear their variables
    memset(&theirID, 0, sizeof(theirID));
    address = retrieveContact(address, &theirTime, &theirID);

      clear();
      text_size(SMALL);
      oledprint("%d\n%d\n", theirTime, theirID);
      //pause(1000);

    sprintf(sendString, "%d,%d", theirTime, theirID);

    dprint(port, "%32s\n", sendString);
  }

  ee_writeShort(0, USER_COUNT_ADDRESS);
  ee_writeShort(0, SERVER_COUNT_ADDRESS);
}
