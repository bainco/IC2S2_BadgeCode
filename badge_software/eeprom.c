/**
EEPROM.c
@Author Connor Bain

This file contains all necessary function definitons and constants for the
IC2S2 conference badges to be able to burn and retrieve contacts to/from EEPROM.
*/

#include "simpletools.h"   // Include simpletools library
#include "badgetools.h"    // Include badgetools library
#include "eeprom.h"        // Include EEPROM header
#include "ID_Info.h"       // Include ID info header
/**** WRITE METHODS ****/

/** storeUserCount(short count)

Method to store the user interaction count.

@args count (short) to store in memory
*/
void storeUserCount(short count) {
  ee_writeShort(count, USER_COUNT_ADDRESS);
}

/** storeServerCount(short count)

Method to store the server interaction count.

@args count (short) to store in memory
*/
void storeServerCount(short count) {
  ee_writeShort(count, SERVER_COUNT_ADDRESS);
}

/** storeContact(int timeVal, short idVal)

Method to store an interaction into EEPROM.

@args timeVal (int) timestamp of interaction
      idVal (short) the id of the interactant

@returns address (int) of next open memory slot
*/
int storeContact(int timeVal, short idVal) {

   // Calculate next open memory slot
   unsigned int addr = MEM_START_ADDRESS + (retrieveCount() * 6);

   // Simple bounds check
   if(addr < (TIME_ADDRESS - 6)) {
      ee_writeInt(timeVal, addr);
      addr += 4;
      ee_writeShort(idVal, addr);
      addr += 2;
      //int memRemaining = 65535 - (addr);
      //memRemaining /= 6;

      // Cases based on the ID values
      if (idVal < STAFF_ID_UPPER_LIMIT) {
         // THIS IS A STAFF BADGE
         // STUB
      }
      else if (idVal < USER_ID_UPPER_LIMIT) {
         // THIS IS A USER BADGE
         storeUserCount(retrieveUserCount() + 1);
      }
      else if (idVal < SERVER_ID_UPPER_LIMIT) {
         // THIS IS A SERVER BADGE
         storeServerCount(retrieveServerCount() + 1);
      }
   }
   // Otherwise, we've got a out of memory problem
   else {
      clear();
      oledprint("ERROR!\n");
      pause(1000);
   }
   return addr;
}
/** END WRITE METHODS **/


/******** READ METHODS *********/

/** retrieveUserCount()

Method to retrieve the user interaction count.

@returns number (short) of user interactions
*/
short retrieveUserCount() {
   // If no count is stored, initialize
   if (ee_readShort(USER_COUNT_ADDRESS) == -1) {
     storeServerCount(0);
   }
   return ee_readShort(USER_COUNT_ADDRESS);
}

/** retrieveServerCount()

Method to retrieve the server interaction count.

@returns number (short) of server interactions
*/
short retrieveServerCount() {

   // If no count is stored, initialize
   if (ee_readShort(SERVER_COUNT_ADDRESS) == -1) {
      storeServerCount(0);
   }
   return ee_readShort(SERVER_COUNT_ADDRESS);
}


/** retrieveCount()

Method to retrieve the total interaction count. Just calls the other two methods.

@returns number (short) of total interactions
*/
short retrieveCount() {
  return retrieveServerCount() + retrieveUserCount();
}

/** retrieveContact(unsigned int addr, int *timeVal, short *idVal)

Method to retrieve a contact from EEPROM.

@args addr (unsigned int) of the contact in EEPROM
      timeVal (*int) a pointer to a variable to store the retrieved time
      idVal (*short) a pointer to a variable to store the retrieved ID

@returns address (unsigned int) of next contact
*/
unsigned int retrieveContact(unsigned int addr, int *timeVal, short *idVal) {
  if(addr < (TIME_ADDRESS - 6)) {
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
