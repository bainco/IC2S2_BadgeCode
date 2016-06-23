#!/usr/bin/env python
#
# Copyright 2007 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import webapp2
import jinja2
import logging
import os
import string
from google.appengine.ext import ndb

jinja_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(os.path.dirname(__file__) + "/templates"))

from datetime import datetime, timedelta

class Attendee(ndb.Model):
    date = ndb.DateTimeProperty(auto_now_add=True)
    badgeID = ndb.IntegerProperty(required=True)
    firstName = ndb.StringProperty(required=True)
    lastName = ndb.StringProperty(required=True)
    email = ndb.StringProperty(required=True)
    position = ndb.StringProperty(required=True)
    affiliation = ndb.StringProperty(required=True)
    discipline = ndb.StringProperty(required=False)
    degree = ndb.StringProperty(required=False)
    survey_answer0 = ndb.IntegerProperty(required=True)
    survey_answer1 = ndb.IntegerProperty(required=True)
    survey_answer2 = ndb.IntegerProperty(required=True)
    survey_answer3 = ndb.IntegerProperty(required=True)
    sonicq_0 = ndb.IntegerProperty(required=True)
    sonicq_1 = ndb.IntegerProperty(required=True)
    sonicq_2 = ndb.IntegerProperty(required=True)
    sonicq_3 = ndb.IntegerProperty(required=True)
    sonicq_4 = ndb.IntegerProperty(required=True)
    sonicq_5 = ndb.IntegerProperty(required=True)

class MyHandler(webapp2.RequestHandler):
    "Setup self.user and self.templateValues values."
    def setup(self):
        self.templateValues = {}

    def render(self, afile):
        "Render the given file"
        template = jinja_environment.get_template(afile)
        self.response.out.write(template.render(self.templateValues))

class MainHandler(MyHandler):
    def get(self): #/ Ask user to login or show him links
        self.setup()
        self.templateValues['title'] = 'IC2S2 Registration'
        self.render('main.html')

class DownloadHandler(MyHandler):
    def fixNames(self, tempLastName, tempFirstName):
        while (len(tempFirstName) < len(tempLastName)):
            tempFirstName = tempFirstName + " "

        while (len(tempLastName) < len(tempFirstName)):
            tempLastName = tempLastName + " "

        tempFirstName = tempFirstName + "  "
        tempLastName = tempLastName + "  "

        return (tempFirstName, tempLastName)

    def generate_header(self, badgeID, firstName, lastName, survey_answer0, survey_answer1, survey_answer2, survey_answer3):

        if (len(firstName) > 8) or (len(lastName) > 8):
            (firstName, lastName) = self.fixNames(str(lastName), str(firstName))

        current_time_in_utc = datetime.utcnow()
        result = current_time_in_utc + timedelta(hours=-5)

        theTime = int ((result - datetime(1970,1,1)).total_seconds())

        lineTime = "const int MY_INIT_TIME = " + str(theTime) + ";\n"
        lineID = "const int MY_ID = " + str(badgeID) + ";\n"
        lineFName = "const char MY_FIRST_NAME[32] = \"" + firstName +"\";\n"
        lineLName = "const char MY_LAST_NAME[32] = \"" + lastName +"\";\n"

        lineSAnswers = "const int MY_ANSWERS[NUM_QUESTIONS] = {" + str(survey_answer0) + "," + str(survey_answer1) + "," + str(survey_answer2) + "," + str(survey_answer3) + "};\n"

        return (lineTime + lineID + lineFName + lineLName + lineSAnswers)

    def post(self, num):
        printable = set(string.printable)

        firstName = self.request.get('firstName')
        lastName = self.request.get('lastName')
        email = self.request.get('email')
        position = self.request.get('position')
        affiliation = self.request.get('affiliation')
        degree = self.request.get('degree')
        discipline = self.request.get('discipline')

        sonicq_0 = int(self.request.get('sonicq_0'))
        sonicq_1 = int(self.request.get('sonicq_1'))
        sonicq_2 = int(self.request.get('sonicq_2'))
        sonicq_3 = int(self.request.get('sonicq_3'))
        sonicq_4 = int(self.request.get('sonicq_4'))
        sonicq_5 = int(self.request.get('sonicq_5'))

        firstName = ''.join(filter(lambda x: x in printable, firstName))
        lastName = ''.join(filter(lambda x: x in printable, lastName))
        position = ''.join(filter(lambda x: x in printable, position))
        affiliation = ''.join(filter(lambda x: x in printable, affiliation))
        degree = ''.join(filter(lambda x: x in printable, degree))
        discipline = ''.join(filter(lambda x: x in printable, discipline))

        survey_answer0 = int(self.request.get('survey_answer0'))
        survey_answer1 = int(self.request.get('survey_answer1'))
        survey_answer2 = int(self.request.get('survey_answer2'))
        survey_answer3 = int(self.request.get('survey_answer3'))

        count = Attendee.query().count()
        if count:
            badgeID = 1000 + count
        else:
            badgeID = 1000

        theAttendee = Attendee(badgeID = badgeID, firstName = firstName, lastName = lastName, email = email, position = position, discipline = discipline, affiliation = affiliation, degree = degree, sonicq_0 = sonicq_0, sonicq_1 = sonicq_1, sonicq_2 = sonicq_2, sonicq_3 = sonicq_3, sonicq_4 = sonicq_4, sonicq_5 = sonicq_5, survey_answer0 = survey_answer0, survey_answer1 = survey_answer1, survey_answer2 = survey_answer2, survey_answer3 = survey_answer3)

        attendeeKey = theAttendee.put()
        url_string = attendeeKey.urlsafe()
        logging.info(theAttendee)

        self.redirect('/Download/%s' % url_string)

    def get(self, keyURL):
        self.setup()
        attendeeKey = ndb.Key(urlsafe=keyURL)
        theAttendee = attendeeKey.get()

        fileName = str(theAttendee.lastName).replace(" ", "_").replace("'", "_")
        fileName = fileName + str(theAttendee.firstName).replace(" ", "_").replace("'", "_")

        self.response.headers['Content-Type'] = 'application/octet-stream'
        self.response.headers["Content-Disposition"] = 'attachment; filename=' + str(fileName) + '.h'
        self.response.headers['Cache-Control'] = 'no-cache'

        header_string = self.generate_header(theAttendee.badgeID, theAttendee.firstName, theAttendee.lastName, theAttendee.survey_answer0, theAttendee.survey_answer1, theAttendee.survey_answer2, theAttendee.survey_answer3)
        self.response.out.write(header_string)
        return

class AttendeeHandler(MyHandler):
    def post(self, num):
        self.setup()
        firstName = self.request.get('firstName')
        lastName = self.request.get('lastName')
        email = self.request.get('email')
        affiliation = self.request.get('affiliation')
        position = self.request.get('position')
        degree = self.request.get('degree')
        discipline = self.request.get('discipline')

        sonicq_0 = int(self.request.get('sonicq_0'))
        sonicq_1 = int(self.request.get('sonicq_1'))
        sonicq_2 = int(self.request.get('sonicq_2'))
        sonicq_3 = int(self.request.get('sonicq_3'))
        sonicq_4 = int(self.request.get('sonicq_4'))
        sonicq_5 = int(self.request.get('sonicq_5'))

        survey_answer0 = int(self.request.get('survey_answer0'))
        survey_answer1 = int(self.request.get('survey_answer1'))
        survey_answer2 = int(self.request.get('survey_answer2'))
        survey_answer3 = int(self.request.get('survey_answer3'))

        self.templateValues['firstName'] = firstName
        self.templateValues['lastName'] = lastName
        self.templateValues['email'] = email
        self.templateValues['position'] = position
        self.templateValues['affiliation'] = affiliation
        self.templateValues['degree'] = degree
        self.templateValues['discipline'] = discipline

        self.templateValues['sonicq_0'] = sonicq_0
        self.templateValues['sonicq_1'] = sonicq_1
        self.templateValues['sonicq_2'] = sonicq_2
        self.templateValues['sonicq_3'] = sonicq_3
        self.templateValues['sonicq_4'] = sonicq_4
        self.templateValues['sonicq_5'] = sonicq_5

        self.templateValues['survey_answer0'] = survey_answer0
        self.templateValues['survey_answer1'] = survey_answer1
        self.templateValues['survey_answer2'] = survey_answer2
        self.templateValues['survey_answer3'] = survey_answer3

        self.render('confirm.html')

app = webapp2.WSGIApplication([
    ('/Attendee/(.*)', AttendeeHandler),
    ('/Download/(.*)', DownloadHandler),
    ('/', MainHandler)
], debug=True)
