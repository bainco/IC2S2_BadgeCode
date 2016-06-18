#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library
#include "eeprom.h"

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
