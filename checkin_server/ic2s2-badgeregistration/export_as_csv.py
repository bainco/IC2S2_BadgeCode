# Run from GAE remote API:
# 	{GAE Path}\remote_api_shell.py -s {YourAPPName}.appspot.com
# 	import export_as_csv

import csv
from main import Attendee
from datetime import datetime, timedelta

def exportToCsv(query, csvFileName, delimiter):
    #with open(csvFileName, 'wb') as csvFile:
    totalRowsSaved = 0
    csvWriter = 0 #csv.writer(csvFile, delimiter=delimiter, quotechar='"', quoting=csv.QUOTE_MINIMAL)
    writeHeader(csvWriter)

    items = query.fetch()
    currentRows =0
    for item in items:
        saveItem(csvWriter, item)
        currentRows += 1

    totalRowsSaved += currentRows
    print 'Saved ' + str(totalRowsSaved) + ' rows'
    print 'Finished saving all rows.'

def writeHeader(csvWriter):
	print(['date', 'badgeID', 'firstName', 'lastName', 'email', 'affiliation', 'position', 'discipline', 'degree','sonicq_0', 'sonicq_1', 'sonicq_2', 'sonicq_3', 'sonicq_4', 'sonicq_5', 'survey_answer0', 'survey_answer1', 'survey_answer2', 'survey_answer3']) #Output csv header

def saveItem(csvWriter, item):
	print([item.date, item.badgeID, item.firstName, item.lastName, item.email, item.affiliation, item.position, item.discipline, item.degree, item.sonicq_0, item.sonicq_1, item.sonicq_2, item.sonicq_3, item.sonicq_4, item.sonicq_5, item.survey_answer0, item.survey_answer1, item.survey_answer2, item.survey_answer3]) # Save items in preferred format

query = Attendee.gql("ORDER BY date", produce_cursors=True) #Query for items

current_time_in_utc = datetime.utcnow()
result = current_time_in_utc + timedelta(hours=-5)
theTime = int ((result - datetime(1970,1,1)).total_seconds())

exportToCsv(query, 'Attendee-' + str(theTime) + '-dump.csv', ',')
