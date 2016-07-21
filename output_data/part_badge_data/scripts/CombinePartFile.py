# Author: Connor Bain
# Script to take a group of badge output CSVs and combine them into a single one

import csv, sys, os

for fFileObj in os.walk("pdata-split/"):
   theFilesList = fFileObj[2]
   break

participantList = {}
badgeCount = 0
interactionCount = 0
theID = -1
theList = []

for aFile in theFilesList:
    with open('pdata-split/' + aFile, 'rb') as csvfile:
        singleFile = csv.reader(csvfile, delimiter=',', quotechar='"')
        for row in singleFile:
            if row == []:
                continue
            if row[0] != 'MY_ID':
                theList.append({'timestamp':row[0], 'badgeID':row[1]})
                interactionCount = interactionCount + 1

            else:
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
    else:
        print "DUPLICATE WARNING"
        tempList = participantList[theID]
        tempList = tempList + theList
        participantList[theID] = tempList
        theList = []
        #print participantList[theID]

print badgeCount
print interactionCount

with open('pdata-all.csv', 'wb') as csvfile:
    outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    for badgeID, interactions in participantList.items():
        badgeCount = badgeCount - 1
        outwriter.writerow(['MY_ID'])
        outwriter.writerow([badgeID])
        outwriter.writerow(['MY_DATA'])
        for item in interactions:
            interactionCount = interactionCount - 1
            outwriter.writerow([item['timestamp'], item['badgeID']])

print badgeCount
print interactionCount

# Code for getting a list of files in a directory
#theFilesList = []
#for fFileObj in os.walk(JS_DIRECTORY):
#   theFilesList = fFileObj[2]
#   break
