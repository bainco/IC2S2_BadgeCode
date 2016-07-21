# Author: Connor Bain
# Script to take a single badge output CSV from multiple badges and split it
# into n badge files.

import csv, sys

participantList = {}
badgeCount = 0
interactionCount = 0
myInteractionCount = 0
theID = -1
theList = []
with open('pdata-all.csv', 'rb') as csvfile:
    singleFile = csv.reader(csvfile, delimiter=',', quotechar='"')

    for row in singleFile:
        if row[0] != 'MY_ID':
            theList.append({'order_num': myInteractionCount, 'timestamp':row[0], 'badgeID':row[1]})
            myInteractionCount = myInteractionCount + 1
            interactionCount = interactionCount + 1

        else:
            if theID != -1:
                if len(theList) == 0:
                    print "NO DATA for", str(theID)
                if theID not in participantList.keys():
                    participantList[theID] = theList
                    theList = []
                    myInteractionCount = 0
                    badgeCount = badgeCount + 1
                else:
                    print "DUPLICATE WARNING"
                    tempList = participantList[theID]
                    tempList = tempList + theList
                    participantList[theID] = tempList
                    theList = []
                    myInteractionCount = 0
                    #print participantList[theID]

            theID = singleFile.next()[0]
            #print theID
            if singleFile.next()[0] != 'MY_DATA':
                print "CORRUPT DATA ERROR."
                sys.exit(0)
            #else:
                #print "Reading data for ID ", str(theID)
# Deal with end condition
if len(theList) == 0:
    print "NO DATA for", str(theID)
if theID not in participantList.keys():
    participantList[theID] = theList
    theList = []
    badgeCount = badgeCount + 1
    myInteractionCount = 0
else:
    print "DUPLICATE WARNING"
    tempList = participantList[theID]
    tempList = tempList + theList
    participantList[theID] = tempList
    theList = []

print badgeCount
print interactionCount

allInteractions = []

with open('all_to_all.csv', 'wb') as csvfile:
    outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    for myID, theirInteractions in participantList.items():
        for item in theirInteractions:
            outwriter.writerow([myID, item['badgeID']])
