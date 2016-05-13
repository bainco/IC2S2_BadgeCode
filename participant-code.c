#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library  

short MY_ID = 128;
char MY_NAME[16] = "IRONMAN";

unsigned int COUNTADDRESS = 33532;
unsigned int MEM_START_ADDRESS = 33534;

short theirID;
unsigned int theirTime;

void sendBeacon();
void listen();

void storeCount(short count);
short retrieveCount(); // Actually returns the count. No referencing.

int storeContact(unsigned int timeVal, short idVal);
int retrieveContact(int addr, unsigned int *timeVal, short *idVal);

void print_all_contacts();


void main()                                  // Main function
{
  badge_setup();                             // Call badge setup
  
  text_size(SMALL);
  oledprint("IR Send");
  cursor(0, 3);
  oledprint("%s", MY_NAME);
  cursor(0, 4);
  oledprint("P17 to CHECK-IN");
  
  while(1)
  {
    int states = buttons();
       
    switch(states)
    {
      case 0b0100000:
        irclear();
        listen();
        rgbs(OFF, OFF);
        break;
        
      case 0b0000100:                         // P25 Pressed - upload all contacts
        clear();
        oledprint("Upload  to      computer");
        print_all_contacts();
        oledprint("Done");
        pause(250);
        clear();
        text_size(SMALL);
        oledprint("IR Send");
        cursor(0, 3);
        oledprint("%s", MY_NAME);
        cursor(0, 4);
        oledprint("P17 to CHECK-IN");
        break;    
        
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
          text_size(SMALL);
          oledprint("IR Send");
          cursor(0, 3);
          oledprint("%s", MY_NAME);
          cursor(0, 4);
          oledprint("P17 to CHECK-IN");        }          
        else                                   // No, don't erase
        {
          clear();
          text_size(SMALL);
          oledprint("IR Send");
          cursor(0, 3);
          oledprint("%s", MY_NAME);
          cursor(0, 4);
          oledprint("P17 to CHECK-IN");
          pause(250);
        } 
        break;
        
       default:
        break; 
    }          
  }      
}  

void listen() {
 while(1) { 
    rgbs(GREEN, GREEN);
    memset(&theirID, 0, sizeof(theirID));        // Clear their variables
    memset(&theirTime, 0, sizeof(theirTime));
    
    irscan("%d%d", &theirTime, &theirID);
    
    if(theirID > 0) {
       rgbs(CYAN, CYAN);
       print("Heard from... id: %d ,", theirID);
       print("time: %d\n", theirTime); 
       
       clear();
       
       oledprint("Check-in at %d");
       storeContact(theirTime, theirID); 
       sendBeacon();
       pause(500);
       clear();
       oledprint("IR Send");
       cursor(0, 3);
       oledprint("%s", MY_NAME);
       text_size(SMALL);
       cursor(0, 4);
       oledprint("P17 to CHECK-IN");
       break;
    }
    pause(50);   
    rgbs(OFF, OFF);   
  }
  rgbs(OFF, OFF);    
}  


void sendBeacon() {
  for(int i = 0; i < 5; i++) {
    rgbs(RED, RED);
    irprint("%d\n%16s\n", MY_ID, MY_NAME);
    pause(100);
    rgbs(OFF, OFF);
  }    
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