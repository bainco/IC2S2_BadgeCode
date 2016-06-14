#define NUM_QUESTIONS ((short) 4)
#define NUM_ANSWERS ((short) 5)

const char SURVEY_Q[NUM_QUESTIONS][64] = {
//S              ES              ES              ES              E
 "Which type of   fame would you  like?",
 "Which technologydo you most lookforward to?",
 "Which TV show   world would you live in?",
 "Which music     genre do you    prefer?"
 };

// DISPLAY IS 17 CHARS WIDE
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
