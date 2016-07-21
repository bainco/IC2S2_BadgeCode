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
    #print participantList[theID]

print badgeCount
print interactionCount


allInteractions = []
for partID, theirInteractions in participantList.items():
    # Their interactions is a list of their interactions in chrono order
    prevTime = 0

    iterator = theirInteractions.__iter__()
    for eachInteraction in iterator:
        while iterator['timestamp'] < prevTime:
        if eachInteraction['timestamp'] < prevTime:
            # THE CLOCK IS BEHIND SOMEHOW... use previous time as lower bound
            newInteraction = Interaction(prevTime, eachInteraction['timestamp'], partID, eachInteraction['badgeID'])
        else:
            newInteraction = Interaction(eachInteraction['timestamp'], eachInteraction['timestamp'], partID, eachInteraction['badgeID'])
            prevTime = eachInteraction['timestamp']

    allInteractions.append()


    print theirInteractions

class Interaction:
   def __init__(self, tLB = -1, tUB = -1, idA = -1, idB = -1):
       self.time_lower_bound = tLB
       self.time_upper_bound = tUB
       self.ID_A = idA
       self.ID_B = idB
