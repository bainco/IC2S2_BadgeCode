#include "simpletools.h"
#include "badgetools.h"
#include "datetime.h"                         // Include datetime library


datetime dt = {2016, 5, 10, 5, 5, 00};  // Set up the date/time
unsigned int et;

int COUNTADDRESS = 33532;
int MEM_START_ADDRESS = 33534;

short MY_LOCATION_ID = 1;

short recentID = 15;

short theirID;
char theirName[16];

int states;

void storeCount(short count);
short retrieveCount(); // Actually returns the count. No referencing.

int storeContact(unsigned int timeVal, short idVal);
int retrieveContact(int addr, unsigned int *timeVal, short *idVal);

void print_all_contacts();

void sendBeacon(unsigned int et);
void listen(unsigned int et);

void listen(unsigned int et) {

     memset(&theirID, 0, sizeof(theirID));        // Clear their variables
     memset(&theirName, 0, sizeof(theirName));        // Clear their variables


     //rgbs(GREEN, GREEN);                          // Signal transmitting
     irscan("%d%16s", &theirID, &theirName);

     if(strlen(theirName) > 0) {

       print("Heard from... id: %d ", theirID);
       print("name: %16s\n", theirName);

       if(theirID != recentID) {
        recentID = theirID;

        storeContact(et, theirID);

        print("Saving... timestamp: %d, id: %d\n", et, theirID);

        rgbs(CYAN, CYAN);
        clear();
        oledprint("Welcome  %s", theirName);
        pause(500);
        clear();
        oledprint("Keynote Check-in");
       }

     }
     rgbs(OFF, OFF);                          // Signal transmittingå
}

void sendBeacon(unsigned int et) {

  // rgbs(RED, RED);                          // Signal transmitting
   irprint("%d\n%d\n", et, MY_LOCATION_ID); // Transmit epoch time
   rgbs(OFF, OFF);                          // Finish transmitting
}

void main()
{
  badge_setup();
  text_size(LARGE);
  oledprint("Keynote Check-in");
  dt_run(dt); 
  rgbs(OFF, OFF);                                 // Use to start system timer

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
        clear();
        oledprint("Keynote Check-in");
        continue;                             // Back to while(1)
      case 0b1000000:                         // OSH pressed, erase EEPROM
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
          clear();
          oledprint("Keynote Check-in");
        }
        else                                   // No, don't erase
        {
          clear();
          oledprint("Keynote Check-in");
          pause(250);
        }
        break;                               // Continue through while(1)
      default:
        break;                               // Continue through while(1)
    }

    dt = dt_get();
    et = dt_toEt(dt); // Convert from date time to epoch time
    irclear();
    sendBeacon(et);
    pause(50);
    listen(et);
  }
}

void print_all_contacts() {

  short currentCount = retrieveCount();

  unsigned int t;
  short id;
  int address = MEM_START_ADDRESS;

  for(int i = 0; i < currentCount; i++)
  {
    address = retrieveContact(address, &t, &id);
    print("%d, %d\n", t, id);
  }
}

void storeCount(short count) {
  ee_writeShort(count, COUNTADDRESS);
}

short retrieveCount() {

  short returnValue = ee_readShort(COUNTADDRESS);
  // If count is unitialized, set it to 0
  if (returnValue == -1) {
    storeCount(0);
    returnValue = 0;
  }
  return returnValue;
}

int storeContact(unsigned int timeVal, short idVal)
{

   short currentCount = retrieveCount();
   unsigned int addr = MEM_START_ADDRESS + (currentCount * 6);

  if(addr < (65536 - 6))
  {
    ee_writeInt(timeVal, addr);
    addr += 4;
    ee_writeShort(idVal, addr);
    addr += 2;
    int memRemaining = 65535 - (addr);
    memRemaining /= 6;

    short newCount = retrieveCount() + 1;
    storeCount(newCount);
    print("Current Contact Count: %d\n", newCount);

    //print("Contacts left = %d\n", memRemaining);
  }
  else
  {
    print("Could not store, address too high!\n");
  }
  return addr;
}

int retrieveContact(int addr, unsigned int *timeVal, short *idVal)
{
  //print("addr = %d   ", addr);
  if(addr < (65536 - 6))
  {
    *timeVal = ee_readInt(addr);
    //print("*timeVal = %d, ", *timeVal);
    addr += 4;
    *idVal = ee_readShort(addr);
    //print("*idVal = %d, ", *idVal);
    addr += 2;
  }
  else
  {
    oledprint("Wrong address!\n");
  }
  return addr;
}