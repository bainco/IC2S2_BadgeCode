# Run from GAE remote API:
# 	{GAE Path}\remote_api_shell.py -s {YourAPPName}.appspot.com
# 	import export_as_csv

import csv
from main import Attendee

def exportToCsv(query, csvFileName, delimiter):
	with open(csvFileName, 'wb') as csvFile:
		totalRowsSaved = 0
		csvWriter = csv.writer(csvFile, delimiter=delimiter, quotechar='"', quoting=csv.QUOTE_MINIMAL)
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
	csvWriter.writerow(['date', 'badgeID', 'firstName', 'lastName', 'email', 'affiliation', 'position', 'discipline', 'degree', 'survey_answer0', 'survey_answer1', 'survey_answer2', 'survey_answer3']) #Output csv header

def saveItem(csvWriter, item):
	csvWriter.writerow([item.date, item.badgeID, item.firstName, item.lastName, item.email, item.affiliation, item.position, item.discipline, item.degree, item.survey_answer0, item.survey_answer1, item.survey_answer2, item.survey_answer3]) # Save items in preferred format

query = Attendee.gql("ORDER BY date", produce_cursors=True) #Query for items
exportToCsv(query, 'myExport.csv', ',')
