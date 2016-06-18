// THESE ARE ADDRESSES SO THEY'RE INTS OF LOCATIONS IN MEMORY
static const unsigned int SERVER_COUNT_ADDRESS = 33532;
static const unsigned int USER_COUNT_ADDRESS = 33534;
static const unsigned int TIME_ADDRESS = 65531;
static const unsigned int MEM_START_ADDRESS = 33536;

void storeServerCount(short count);
void storeUserCount(short count);
short retrieveCount(); // Actually returns the count. No referencing.
short retrieveUserCount(); // Actually returns the count. No referencing.
short retrieveServerCount(); // Actually returns the count. No referencing.

int storeContact(unsigned int timeVal, short idVal);
int retrieveContact(int addr, unsigned int *timeVal, short *idVal);
