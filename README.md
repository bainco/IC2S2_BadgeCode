# To Setup a Registration Computer:
1. Install FTDI Serial Drivers
2. Install SimpleIDE
3. Copy badge-code folder to ~/Desktop
4. Install folder action
5. Enable folder action on ~/Downloads
6. Disable Safari Auto-fill


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
