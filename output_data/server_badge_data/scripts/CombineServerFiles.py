import csv, sys, os

for fFileObj in os.walk('../sdata-split/'):
   theFilesList = fFileObj[2]
   break

serverList = {}
badgeCount = 0
interactionCount = 0

list_of_server_names = []

for aFile in theFilesList:
   with open('../sdata-split/' + aFile, 'rb') as csvfile:
       singleFile = csv.reader(csvfile, delimiter=',', quotechar='"')
       parsedFileName = aFile.split('.')[0].split('-')
       loadID = int(parsedFileName[1])
       list_of_server_names.append((loadID, parsedFileName[2], parsedFileName[3].replace('_', " ")))

       expectedCount = 0
       badgeCount += 1
       loadInteractions = []
       for row in singleFile:
           if "Current" in row[0]:
               expectedCount = int(row[0].split(':')[1].strip())
               continue

           loadInteractions.append({'timestamp':int(row[0]), 'badgeID': int(row[1])})
           interactionCount += 1

       if expectedCount != len(loadInteractions):
           print "WARNING"
       serverList[loadID] = loadInteractions

print badgeCount
print interactionCount

with open('sdata-all.csv', 'wb') as csvfile:
    outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    for badgeID, interactions in serverList.items():
        badgeCount = badgeCount - 1
        outwriter.writerow(['MY_ID'])
        outwriter.writerow([badgeID])
        outwriter.writerow(['MY_DATA'])
        for item in interactions:
            interactionCount = interactionCount - 1
            outwriter.writerow([item['timestamp'], item['badgeID']])

print badgeCount
print interactionCount

with open('server-locations-map.csv', 'wb') as csvfile:
    outwriter = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    for server in list_of_server_names:
        outwriter.writerow([server[0], server[1], server[2]])
