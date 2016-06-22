/**
EEPROM.h
@Author Connor Bain

This file contains all necessary function definitons and constants for the
IC2S2 conference badges to be able to burn and retrieve contacts to/from EEPROM.
*/

// THESE ARE ADDRESSES SO THEY'RE UNSIGNED INT LOCATIONS IN MEMORY
static const unsigned int SERVER_COUNT_ADDRESS = 33532; // stores server interaction count
static const unsigned int USER_COUNT_ADDRESS = 33534;   // stores user interaction count
static const unsigned int TIME_ADDRESS = 65531;         // stores current time
static const unsigned int MEM_START_ADDRESS = 33536;    // indicates start of memory for contacts

// WRITE METHODS
void storeServerCount(short count);          // Method to store a server interaction count
void storeUserCount(short count);            // Method to store the user interaction count
int storeContact(int timeVal, short idVal);  // Method to store a contact

// READ METHODS
short retrieveCount();               // Method to return total interaction count
short retrieveUserCount();           // Method to retrieve user interaction count
short retrieveServerCount();         // Method to retrieve server interaction count
unsigned int retrieveContact(unsigned int addr, int *timeVal, int *idVal);   // Method to retrieve a contact
