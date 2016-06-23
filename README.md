# To Setup a Registration Computer:
1. Install FTDI Serial Drivers
2. Install SimpleIDE
3. Copy badge-code folder to ~/Desktop
4. Install folder action
5. Enable folder action on ~/Downloads
6. Disable Safari Auto-fill
7. Disable Screen Saver
8. (Energy Saver) Make sure display is always on

# To Register:
0. MAKE SURE NAMES.CSV IS UPDATED
1. Plug in badge
2. Navigate to www.ic2s2-badgeregistration.appspot.com
3. Fill out form; Click Submit
4. Confirm info; Click CONFIRM AND BURN TO BADGE
5. Wait for"Burn Successful" Notification
6. Hit "DONE" button
7. Unplug badge

# To Harvest Data from App Engine
0. /usr/local/google_appengine/remote_api_shell.py -s ic2s2-badgeregistration.appspot.com (Note: this requries *.json credential from GAE set in .bash_profile)
1. import export_as_csv

# Registration Common Problems
0. "RAM Verification Failed" - Solved by burning small sample program to badge and then reburning program.
1. "RAM Verification\n Check-sum invalid" - This ocurred on the server badges and went away when I removed the sound.h header. I think it might have something to do with recompiling the files (it may be auto-recognizing changes and delta-compiling instead of full-compiling).
2. Buttons don't work - We think the solution is to just scrub the buttons with a cloth. This might be due to film from the manufacturing process.
3. "No Badge Connected" - The Mac FTDI driver doesn't cooperate sometimes. You can make the driver crash by opening a screen <code>screen /dev/cu.usbserial.<port> 115200</code> and then disconnecting violently (e.g. unplug). It seems like it can't reconcile the open port and doesn't recognize subsequent badges. I tried <code>sudo kextunload /Library/Extension/FTDI<name>.kext</code> but it spits out an error that one instance is still running so you can't unload it. This is fixed by a simple restart. (Note: I've managed to make my personal laptop unable to recognize badges. I've tried unloading the kext, rebuilding the kernel cache, and reinstalling the FTDI drivers. Nothing).
