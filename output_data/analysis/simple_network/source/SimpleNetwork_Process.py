# Author: Connor Bain
# Script to take a single badge output CSV from multiple badges and split it
# into n badge files.

import csv, sys

part_badge_list = {}
def read_pdata():
    part_badge_list.clear()
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
                    if theID not in part_badge_list.keys():
                        part_badge_list[theID] = theList
                        theList = []
                        myInteractionCount = 0
                        badgeCount = badgeCount + 1
                    else:
                        print "DUPLICATE WARNING"
                        tempList = part_badge_list[theID]
                        tempList = tempList + theList
                        part_badge_list[theID] = tempList
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
    if theID not in part_badge_list.keys():
        part_badge_list[theID] = theList
        theList = []
        badgeCount = badgeCount + 1
        myInteractionCount = 0
    else:
        print "DUPLICATE WARNING"
        tempList = part_badge_list[theID]
        tempList = tempList + theList
        part_badge_list[theID] = tempList
        theList = []

    print "Read in", str(badgeCount), "part badges."
    print "Read in", str(interactionCount), "interactions."

server_badge_list = {}
def read_sdata():
    server_badge_list.clear()
    badgeCount = 0
    interactionCount = 0
    myInteractionCount = 0
    theID = -1
    theList = []
    with open('sdata-all.csv', 'rb') as csvfile:
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
                    if theID not in server_badge_list.keys():
                        server_badge_list[theID] = theList
                        theList = []
                        myInteractionCount = 0
                        badgeCount = badgeCount + 1
                    else:
                        print "DUPLICATE WARNING"
                        tempList = server_badge_list[theID]
                        tempList = tempList + theList
                        server_badge_list[theID] = tempList
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
    if theID not in server_badge_list.keys():
        server_badge_list[theID] = theList
        theList = []
        badgeCount = badgeCount + 1
        myInteractionCount = 0
    else:
        print "DUPLICATE WARNING"
        tempList = server_badge_list[theID]
        tempList = tempList + theList
        part_badge_list[theID] = tempList
        theList = []

    print "Read in", str(badgeCount), "part badges."
    print "Read in", str(interactionCount), "interactions."

def write_all_to_csv():
    read_pdata()
    read_sdata()
    writes = 0
    with open('all_interactions.csv', 'wb') as csvfile:
        outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        for myID, theirInteractions in part_badge_list.items():
            for item in theirInteractions:
                outwriter.writerow([myID, item['badgeID']])
                writes += 1

        for myID, theirInteractions in server_badge_list.items():
            for item in theirInteractions:
                outwriter.writerow([myID, item['badgeID']])
                writes += 1
    print "Wrote", int(writes), "interactions"

def write_server_to_csv():
    read_sdata()
    writes = 0
    with open('server_interactions.csv', 'wb') as csvfile:
        outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        for myID, theirInteractions in server_badge_list.items():
            for item in theirInteractions:
                outwriter.writerow([myID, item['badgeID']])
                writes += 1
    print "Wrote", int(writes), "interactions"

def write_part_to_csv():
    read_pdata()
    writes = 0
    with open('part_interactions.csv', 'wb') as csvfile:
        outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        for myID, theirInteractions in part_badge_list.items():
            for item in theirInteractions:
                outwriter.writerow([myID, item['badgeID']])
                writes += 1
    print "Wrote", int(writes), "interactions"
