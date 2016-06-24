# Author
Most of the software and documentation is written by Connor Bain for the IC2S2 conference. Other code contributions are from Parralx (Badge Libraries). Some code was adapted from Corey Brady's previous work on the badges (see BoozeWizardSerialDump.py) and from gergelyorosz's export_as_csv for GAE.

# To Setup a Registration Computer:
1. Install <a href="http://www.ftdichip.com/Drivers/VCP/MacOSX/FTDIUSBSerialDriver_v2_3.dmg">FTDI Serial Drivers (MacOSX-v2.3)</a>
2. Install <a href="http://downloads.parallax.com/plx/software/side/102rc2/Simple-IDE_1-0-2-RC2_MacOS.pkg">SimpleIDE</a>
3. Open SimpleIDE in order to create workspace (in <code>~/Documents</code>)
4. Install Dropbox (user: ic2s2.badges@gmail.com)
5. Ensure libbadgetools, libdatetime, participant-code.c, participant-code.h, survey.h, eeprom.h, eeprom.c, ID_info.h, and badge-burn.command files are synced and intact.
6. Create symlink to dropbox on the desktop using:<code>ln -s ~/Dropbox/ ~/Desktop/badge-code</code>
7. Install folder action. You can do this by opening BadgeBurn-AppleScript in the Dropbox folder.
8. Enable folder action via applescript
9. Disable Safari Auto-fill
10. Disable Screen Saver
11. (Energy Saver) Make sure display is always on

# To Register:
0. MAKE SURE NAMES.CSV IS UPDATED
1. Plug in badge
2. Navigate to www.ic2s2-badgeregistration.appspot.com
3. Fill out form; Click Submit
4. Confirm info; Click CONFIRM AND BURN TO BADGE
5. Wait for "Burn Successful" Notification
6. Hit "DONE" button
7. Unplug badge

# To Harvest Data from App Engine
0. In terminal run <code>/usr/local/google&#95;appengine/remote&#95;api&#95;shell.py -s ic2s2-badgeregistration.appspot.com </code> (Note: this requries <code>&#42;.json</code> credential from GAE (downloaded via the online admin interface) and the <code>export GOOGLE&#95;APPLICATION&#95;CREDENTIALS="&lt;path&#95;to&#95;&#42;.json&gt;"</code> set in <code>.bash_profile</code>)
1. From the remote shell, run <code>import export&#95;as&#95;csv</code>
2. CSV file will be outputted with timestamped date. (NOTE: no idea how this deals with write ops, so be careful)

# To Harvest Data from Server Badges
0. Plug-in server badge via micro-USB to computer with FTDI drivers installed.
1. Open up terminal and execute: <code>screen -L /dev/cu.usbserial.<port> 115200</code>. (115200 is the default baud rate)
2. Once connected, hit P25 on the badge. (We do this 3 times to have a backup)
3. Disconnect from screen using ctrl-a then ctrl-\ and then 'y'.
4. In whatever directory you were in, there will be a file called screenlog.0. That's your data file.

# To Harvest Data from Part Badge
0. Burn BoozeWizard.c to a new badge.
1. Boot up the badge. It will say "ERASING CONTACTS" and then switch to "connecting...".
2. Plug the badge into a computer via usb.
3. Run either <code>python BoozeWizardSerialDump-NL.py</code> or <code>python BoozeWizardSerialDump.py</code. The first will create a single file for all badges scanned, the latter will create a single file (labeled with BadgeID) for every badge scanned.
4. Once connected, the badge will display "HOTSPOT!"
5. Scan any and all badges by "Checking-in" to the new badge. Please hold it there until you see yellow flashing lights.
6. Data will be printed to the appropriate csv file.

# Registration Common Problems
0. "RAM Verification Failed" - Solved by burning small sample program to badge and then reburning program.
1. "RAM Verification\n Check-sum invalid" - This ocurred on the server badges and went away when I removed the sound.h header. I think it might have something to do with recompiling the files (it may be auto-recognizing changes and delta-compiling instead of full-compiling).
2. Buttons don't work - We think the solution is to just scrub the buttons with a cloth. This might be due to film from the manufacturing process.
3. "No Badge Connected" - The Mac FTDI driver doesn't cooperate sometimes. You can make the driver crash by opening a screen <code>screen /dev/cu.usbserial.<port> 115200</code> and then disconnecting violently (e.g. unplug or via ctrl-a,d). It seems like it can't reconcile the open port and doesn't recognize subsequent badges. I tried <code>sudo kextunload /Library/Extension/FTDI<name>.kext</code> but it spits out an error that one instance is still running so you can't unload it. This is fixed by a simple restart. (Note: I've managed to make my personal laptop unable to recognize badges. I've tried unloading the kext, rebuilding the kernel cache, and reinstalling the FTDI drivers. Nothing).
