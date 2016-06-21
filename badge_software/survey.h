/**
survey.h
@Author Connor Bain

This file contains global variables for the IC2S2 Conference Survey.

Note: In text_size(SMALL), the display is 17 chars wide. Use:
//S              E
for spacing.
*/

#define NUM_QUESTIONS ((short) 4) // Number of questions in the survey
#define NUM_ANSWERS ((short) 5)   // Number of answers for each equestion
  

// Array for all of the survey questions
const char SURVEY_Q[NUM_QUESTIONS][64] = {
//S              ES              ES              ES              E
 "Which type of   fame would you  like?",
 "Which technologydo you most lookforward to?",
 "Which TV show   world would you live in?",
 "Which music     genre do you    prefer?"
 };

// Array for all of the possible survey answers
const char SURVEY_Q_ANSWERS[NUM_QUESTIONS][NUM_ANSWERS][32] = {
 {//S              E
  "Million YouTube followers",
 //S              E
  "A cool TED talk",
  "NY Times Cover",
  "Late Show guest",
  "None"
 },
 //S              E
 {"self-driving    cars",
  "a robot butler",
  "Interstellar    travel",
  "Brain computer  interface",
  "Virtual reality"
 },
 {"Mad Men",
  "Game of Thrones",
  "Downton Abbey",
  "Breaking Bad",
  "Star Trek"
 },
 {"Pop",
  "Classical",
  "Jazz",
  "Rock",
  "Musicals"
 }
};
