;; Authors: Connor Bain and David Weintrop

;; Adapted from Corey Brady's Disease Partsim

extensions [ csv ]
globals [answers-list questions-list selected-question first-sick   all-interactions  first-simtick  max-simticks   last-replay-time   last-links-to-show  long-participant-list last-username]

breed [ participants participant ]
breed [ ghosts ghost]


breed [ server-data-turtles server-data-turtle ]
server-data-turtles-own [
 badgeID
 name
 theDay
]

breed [survey-data-turtles survey-data-turtle]
survey-data-turtles-own [
 badgeID
 email
 timestamp
 firstName
 lastName
 degree
 myPosition
 affiliation
 discipline
 sonicq_0 sonicq_1 sonicq_2 sonicq_3 sonicq_4 sonicq_5
 surveyq_0 surveyq_1 surveyq_2 surveyq_3
]

directed-link-breed [ infections infection ]
undirected-link-breed [ connections connection ]

turtles-own [save-label]
participants-own [ id interaction_ids interaction_times]
ghosts-own [ id ]

links-own [ my-times ]

to setup
  reset-ticks
  set questions-list [
    "Which best describes you:"
    "Are your closest collaborators/colleagues:"
    "Is this your first time attending a Computaitonal Social Science event?"
    "When it comes to computational data, tools, and methods, I am:"
    "When it comes to social science theories and methods, I am:"
    "When it comes to computational social science, I am:"
    "Which type of fame would you most prefer?"
    "Which technology are you looking forward to the most?"
    "Which TV show world would you want to live in for a year?"
    "Which of these musical genres do you prefer?"
    "default"
  ]
  set answers-list
  [
  ["More of a computational scientist than a social scientist"
   "More of a Social Scientist than a Computational Social Scientist"
   "A pure Computational Social Scientist"
   ""
   ""
  ]
  ["More Computational Social Scientists than Social Scientists"
   "More Social Scientists than Computational Scientists"
   "An even mix of Social and Computational Scientists"
   ""
   ""
   ]
  ["Yes" "No" "" "" ""]
  ["A novice" "A beginner" "Competent" "Proficient" "An expert"]
  ["A novice" "A beginner" "Competent" "Proficient" "An expert"]
  ["A novice" "A beginner" "Competent" "Proficient" "An expert"]
  ["Having 1,000,000 followers on Youtube"
    "Having a TED talk"
    "Being on the cover of the NY Times"
    "Being a guest on the Late Show"
    ""
   ]
  ["Self-driving cars"
    "A robot butler"
    "Interstellar travel"
    "Brain computer interface"
    "Virtual Reality"
    ]
  ["Mad Men"
    "Game of Thrones"
    "Downton Abbey"
    "Breaking Bad"
    "Star Trek"
    ]
  ["Pop"
    "Classical"
    "Jazz"
    "Rock"
    "Musicals"]
  ["" "" "" "" "" ""]
  ]

  ask patches [ set pcolor white ]

  set choose-a-question "default"
  set selected-question 10
  ask turtles with [(not is-survey-data-turtle? self) and (not is-server-data-turtle? self)] [die]
  set-default-shape participants "cylinder"
  set-default-shape ghosts "cylinder"

  set first-sick nobody

  set all-interactions []
  set last-username 0
  set first-simtick 10000000
  set max-simticks 0
  set long-participant-list []

  load-data-file
  set all-interactions sort-by [item 2 ?1 < item 2 ?2] all-interactions
  redo-network

  reset-ticks
end

to go

  tick
end

to toggle-servers

  ifelse not show-servers? [
  ask ghosts with [id > 1999] [
    ask my-links [
     hide-link
    ]
    ht
  ]

  ][
  ask ghosts with [id > 1999] [
    ask my-links[
      show-link
    ]
      st
  ]
  ]

end

to load-server-file
  file-open "/Users/connorbain/nudropbox/School/CCL/ic2s2-badgecode/badge_data_collection/server_names.csv"
  while [not file-at-end?] [
   let row csv:from-row file-read-line
   create-server-data-turtles 1 [
     set badgeID item 0 row
     set name item 1 row
     set theDay item 2 row
     ht
   ]
  ]
  file-close
end

to load-database-file
  ;  let f "sample_data.csv"
  file-open "/Users/connorbain/nudropbox/School/CCL/ic2s2-badgecode/badge_data_collection/database_dump.csv"
  let headerLine csv:from-row file-read-line
  while [not file-at-end?] [
   let row csv:from-row file-read-line
   create-survey-data-turtles 1 [
    set timestamp item 0 row
    set badgeID item 1 row
    set firstName item 2 row
    set lastName item 3 row
    set email item 4 row
    set affiliation item 5 row
    set myPosition item 6 row
    set discipline item 7 row
    set degree item 8 row
    set sonicq_0 item 9 row
    set sonicq_1 item 10 row
    set sonicq_2 item 11 row
    set sonicq_3 item 12 row
    set sonicq_4 item 13 row
    set sonicq_5 item 14 row
    set surveyq_0 item 15 row
    set surveyq_1 item 16 row
    set surveyq_2 item 17 row
    set surveyq_3 item 18 row

    if sonicq_0 = "" [set sonicq_0 -2]
    if sonicq_1 = "" [set sonicq_1 -2]
    if sonicq_2 = "" [set sonicq_2 -2]
    if sonicq_3 = "" [set sonicq_3 -2]
    if sonicq_4 = "" [set sonicq_4 -2]
    if sonicq_5 = "" [set sonicq_5 -2]
    ht
   ]
  ]
  file-close
end


to load-data-file
;  let f "sample_data.csv"
   file-open "/Users/connorbain/nudropbox/School/CCL/ic2s2-badgecode/badge_data_collection/bw_data.csv"
     let row csv:from-row file-read-line ;; priming read
     while [ not file-at-end? ]
     [
       let my_id 0
       if item 0 row = "MY_ID" [
         set row csv:from-row file-read-line
         set my_id item 0 row

         set row csv:from-row file-read-line
         let my_data item 0 row

         let my_interactions_time []
         let my_interactions_id []
         if my_data = "MY_DATA" [

           set row csv:from-row file-read-line
           while [item 0 row != "MY_ID" and not file-at-end?] [
            ; show row
             let line_time item 0 row
             let line_id item 1 row
             set my_interactions_time lput line_time my_interactions_time
             set my_interactions_id lput line_id my_interactions_id

             set all-interactions lput (list my_id line_id line_time) all-interactions
             if (  line_time < first-simtick ) [ set first-simtick line_time ]
             if (  line_time > max-simticks ) [ set max-simticks line_time ]
             set row csv:from-row file-read-line
           ]
         ]
         show my_id
         ask ghosts with [id = my_id] [die]
         if not any? ghosts with [id = my_id] [
           if not any? participants with [id = my_id] [
           make-a-participant-with-id my_id my_interactions_id my_interactions_time
           ]
         ]
         ;       parse-data-from-line line
       ]
     ];while not at end
  ;if valid file
  file-close
  end

;to parse-data-from-line [ line  ]
;  print line
;  let p position "," line
;  if is-number? p and p > 0 [
;
;    ;;get out the data elements
;    let me substring line 0 p
;    let rest substring line (p + 1) (length line)
;    let q position "," rest
;    let them substring rest 0 q
;    let timestring  (substring rest (q + 1) (length rest))
;
;    let thetime process timestring
;    if not is-number? thetime [ show (word "ERROR in time: " thetime) set thetime 0 ]
;
;    ;add to the interactions record, and extend the max-time if necessary
;    set all-interactions lput (list me them thetime) all-interactions
;    if ( thetime < first-simtick ) [ set first-simtick thetime ]
;    if ( thetime > max-simticks ) [ set max-simticks thetime ]
;    set long-participant-list lput them long-participant-list
;    ifelse (me = last-username)
;      [
;        ask participants with [ id = me ] [ set interactions interactions + 1]
;      ];case of existing
;      [
;        ask participants with [ id = me ] [ show (word "Participant " id " has duplicate data " ) die ]
;        make-a-participant-with-id me them thetime
;        set last-username me
;      ];case of new participant
;  ];if valid line (has a ,)
;end

to move-turtles
 if mouse-down?
 [
   let t min-one-of turtles [ distancexy mouse-xcor mouse-ycor ]
   while [ mouse-down? ]
   [
     ask t [
       setxy mouse-xcor mouse-ycor
     ]
   ]
   stop
 ]
end

to make-a-participant-with-id [ in_id in_interaction_ids in_interaction_times]
  create-participants 1 [
    set color blue
    show "Participant loaded."
    set id in_id
    let candidate-patches  patches with [ abs pxcor < max-pxcor - 2  and abs pycor < max-pycor - 2 ]
    if any? candidate-patches with [not any? participants in-radius 4.2] [ set candidate-patches candidate-patches with [not any? participants in-radius 4.2] ]
    move-to one-of candidate-patches
    set size 1.5
    let padding max (list 0 (.5 + (8 - 4) / 2) )
    let pad ""
    repeat padding [ set pad (word pad " ") ]
    set label (word id pad)
    set interaction_ids in_interaction_ids
    set interaction_times in_interaction_times
    st
  ]
end

to make-a-ghost-with-id [in_id in_task]
  create-ghosts 1 [
    set id in_id
    let candidate-patches  patches with [ abs pxcor < max-pxcor - 2  and abs pycor < max-pycor - 2 ]
    if any? candidate-patches with [not any? participants in-radius 4.2] [ set candidate-patches candidate-patches with [not any? participants in-radius 4.2] ]
    move-to one-of candidate-patches
    set size 1.5
    let padding max (list 0 (.5 + (8 - 4) / 2) )
    let pad ""
    repeat padding [ set pad (word pad " ") ]

   ifelse id > 1999 [
     set shape "square"
     show in_id
     set color lime
     ;set label (word [name] of one-of server-data-turtles with [badgeID = in_id] ", " [substring theDay 0 2] of one-of server-data-turtles with [badgeID = in_id])
    ]
    [set label (word id pad)
          set color grey]

    run in_task

  ]
end

to recolor
  ask participants [

   let response -1
   if choose-a-question = "default" [
    set selected-question 10
    set response -1
   ]
   if choose-a-question = "Best Describes" [
     set selected-question 0
     set response [sonicq_0] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Collaboration" [
     set selected-question 1
     set response [sonicq_1] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "First Time Attendance" [
     set selected-question 2
     set response [sonicq_2] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Computational Experience" [
     set selected-question 3
     set response [sonicq_3] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Social Science Experience" [
     set selected-question 4
     set response [sonicq_4] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Computational Social Science Experience" [
     set selected-question 5
     set response [sonicq_5] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Fame" [
     set selected-question 6
     set response [surveyq_0] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Technology" [
     set selected-question 7
     set response [surveyq_1] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "TV Show" [
     set selected-question 8
     set response [surveyq_2] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]
   if choose-a-question = "Music Genre" [
     set selected-question 9
     set response [surveyq_3] of one-of survey-data-turtles with [badgeID = [id] of myself]
   ]

   if response = -2 [
    set color grey
   ]

   if response = 0 [
     set color blue
   ]
   if response = 1 [
     set color red
   ]
   if response = 2 [
     set color yellow
   ]
   if response = 3 [
     set color white
   ]
   if response = 4 [
     set color magenta
   ]
   if response = -1 [
     set color blue
  ]
  ]
end

to-report process [atime]
  let p position ":" atime
  let hours substring atime 0 p
  let remain substring atime (p + 1) length atime
  let q position ":" remain
  let mins substring remain 0 q
  let secs substring remain (q + 1) length remain
  let answer 0
  ;show (word secs " + 60 * "  mins " + 3600 * " hours)
  carefully [
    set answer read-from-string secs + 60 * read-from-string mins + 3600 * read-from-string hours
  ]
  [
   show error-message
  ]
  report answer
end

to redo-network
  ask links [ die ]
  foreach all-interactions
  [
    ;;non-disease interactions
    let inter ?
    let turtlea one-of turtles with [((not is-survey-data-turtle? self) and (not is-server-data-turtle? self)) and id = item 0 ? ]
    let turtleb one-of turtles with [((not is-survey-data-turtle? self) and (not is-server-data-turtle? self)) and id = item 1 ? ]

    let thetime item 2 ?
    ifelse ( turtlea = nobody or turtleb = nobody ) [
      ifelse turtlea = nobody [
          make-a-ghost-with-id item 0 ? task [create-connection-with turtleb [ set my-times (list thetime)]]
      ][
        make-a-ghost-with-id item 1 ? task [create-connection-with turtlea [ set my-times (list thetime)]]
      ]
      ;show (word "processed interaction " ?)
     ]
    [
      ask turtlea [
        ;let b one-of turtlesb
        carefully [
        ifelse connection-neighbor? turtleb
        [ ask link-with turtleb [ if (not member? thetime my-times) [set my-times lput thetime my-times]  ] ]
        [ create-connection-with turtleb [ set my-times (list thetime)] ]
        ][]
      ]
    ]
  ]
end

to-report my-first-time
  report min my-times
end

to-report my-last-time
  report max my-times
end
@#$#@#$#@
GRAPHICS-WINDOW
330
12
1284
638
32
20
14.5231
1
14
1
1
1
0
0
0
1
-32
32
-20
20
0
0
1
ticks
30.0

BUTTON
15
10
320
55
Refresh Model
setup
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

MONITOR
15
505
160
562
Num Participants
count participants
17
1
14

BUTTON
16
456
322
490
Drag to Arrange Icons
move-turtles
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

MONITOR
215
580
318
637
Interactions
sum [ length my-times ] of connections
17
1
14

MONITOR
170
505
320
562
most-connected
[id] of max-one-of participants [ length interaction_ids ]
17
1
14

BUTTON
175
780
315
813
hide names
ask turtles [ set save-label label  set label \"\"]
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
15
780
165
813
show names
ask turtles [ if label = \"\" [ set label save-label ]]
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
15
820
165
853
Nodes as Dots
ask turtles [ set shape \"dot\" ]
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
175
820
315
853
Nodes as People
ask participants [ set shape \"participant\"  ]\nask ghosts [ set shape \"ghost\"  ]
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
15
419
321
453
Redo Layout
every .01 [layout-spring turtles links .1 world-height / 5 .99\nask turtles[\nrt random 30 - 15\nfd .15\n]\n;ask ghosts [ifelse (id > 1999) [set ycor min-pycor + 5] [set ycor max-pycor - 3]]\n;ask participants [set ycor max-pycor - 9]\n]\n
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

CHOOSER
15
65
170
110
choose-a-question
choose-a-question
"default" "Best Describes" "Collaboration" "First Time Attendance" "Computational Experience" "Social Science Experience" "Computational Social Science Experience" "Fame" "Technology" "TV Show" "Music Genre"
0

BUTTON
175
65
320
110
NIL
recolor
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

MONITOR
115
580
210
637
DB Entries
count survey-data-turtles
17
1
14

BUTTON
325
780
635
825
NIL
load-database-file
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

MONITOR
15
120
320
165
Selected-question
item selected-question questions-list
17
1
11

MONITOR
15
220
320
265
red
item 1 item selected-question answers-list
17
1
11

MONITOR
15
170
320
215
blue
item 0 item selected-question answers-list
17
1
11

MONITOR
15
270
320
315
yellow
item 2 item selected-question answers-list
17
1
11

MONITOR
15
320
320
365
white
item 3 item selected-question answers-list
17
1
11

MONITOR
15
370
320
415
magenta
item 4 item selected-question answers-list
17
1
11

MONITOR
15
580
110
637
Num Ghosts
count ghosts
17
1
14

BUTTON
640
780
767
825
NIL
load-server-file
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

MONITOR
640
830
770
875
NIL
count server-data-turtles
17
1
11

BUTTON
975
665
1097
698
NIL
toggle-servers
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

SWITCH
825
665
972
698
show-servers?
show-servers?
1
1
-1000

PLOT
15
640
315
775
plot 1
NIL
NIL
1000.0
1500.0
0.0
60.0
false
false
"" ""
PENS
"default" 1.0 1 -16777216 true "" "ask participants [plotxy id count my-links]"

BUTTON
355
650
418
683
NIL
go
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

@#$#@#$#@
## WHAT IS IT?

(a general understanding of what the model is trying to show or explain)

## HOW IT WORKS

(what rules the agents use to create the overall behavior of the model)

## HOW TO USE IT

(how to use the model, including a description of each of the items in the Interface tab)

## THINGS TO NOTICE

(suggested things for the user to notice while running the model)

## THINGS TO TRY

(suggested things for the user to try to do (move sliders, switches, etc.) with the model)

## EXTENDING THE MODEL

(suggested things to add or change in the Code tab to make the model more complicated, detailed, accurate, etc.)

## NETLOGO FEATURES

(interesting or unusual features of NetLogo that the model uses, particularly in the Code tab; or where workarounds were needed for missing features)

## RELATED MODELS

(models in the NetLogo Models Library and elsewhere which are of related interest)

## CREDITS AND REFERENCES

(a reference to the model's URL on the web if it has one, as well as any other necessary credits, citations, and links)
@#$#@#$#@
default
true
0
Polygon -7500403 true true 150 5 40 250 150 205 260 250

airplane
true
0
Polygon -7500403 true true 150 0 135 15 120 60 120 105 15 165 15 195 120 180 135 240 105 270 120 285 150 270 180 285 210 270 165 240 180 180 285 195 285 165 180 105 180 60 165 15

arrow
true
0
Polygon -7500403 true true 150 0 0 150 105 150 105 293 195 293 195 150 300 150

box
false
0
Polygon -7500403 true true 150 285 285 225 285 75 150 135
Polygon -7500403 true true 150 135 15 75 150 15 285 75
Polygon -7500403 true true 15 75 15 225 150 285 150 135
Line -16777216 false 150 285 150 135
Line -16777216 false 150 135 15 75
Line -16777216 false 150 135 285 75

bug
true
0
Circle -7500403 true true 96 182 108
Circle -7500403 true true 110 127 80
Circle -7500403 true true 110 75 80
Line -7500403 true 150 100 80 30
Line -7500403 true 150 100 220 30

butterfly
true
0
Polygon -7500403 true true 150 165 209 199 225 225 225 255 195 270 165 255 150 240
Polygon -7500403 true true 150 165 89 198 75 225 75 255 105 270 135 255 150 240
Polygon -7500403 true true 139 148 100 105 55 90 25 90 10 105 10 135 25 180 40 195 85 194 139 163
Polygon -7500403 true true 162 150 200 105 245 90 275 90 290 105 290 135 275 180 260 195 215 195 162 165
Polygon -16777216 true false 150 255 135 225 120 150 135 120 150 105 165 120 180 150 165 225
Circle -16777216 true false 135 90 30
Line -16777216 false 150 105 195 60
Line -16777216 false 150 105 105 60

car
false
0
Polygon -7500403 true true 300 180 279 164 261 144 240 135 226 132 213 106 203 84 185 63 159 50 135 50 75 60 0 150 0 165 0 225 300 225 300 180
Circle -16777216 true false 180 180 90
Circle -16777216 true false 30 180 90
Polygon -16777216 true false 162 80 132 78 134 135 209 135 194 105 189 96 180 89
Circle -7500403 true true 47 195 58
Circle -7500403 true true 195 195 58

circle 2
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240

cow
false
0
Polygon -7500403 true true 200 193 197 249 179 249 177 196 166 187 140 189 93 191 78 179 72 211 49 209 48 181 37 149 25 120 25 89 45 72 103 84 179 75 198 76 252 64 272 81 293 103 285 121 255 121 242 118 224 167
Polygon -7500403 true true 73 210 86 251 62 249 48 208
Polygon -7500403 true true 25 114 16 195 9 204 23 213 25 200 39 123

cylinder
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 false false 0 0 300

dot
false
0
Circle -7500403 true true 30 0 240

face happy
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 255 90 239 62 213 47 191 67 179 90 203 109 218 150 225 192 218 210 203 227 181 251 194 236 217 212 240

face neutral
false
0
Circle -7500403 true true 8 7 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Rectangle -16777216 true false 60 195 240 225

face sad
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 168 90 184 62 210 47 232 67 244 90 220 109 205 150 198 192 205 210 220 227 242 251 229 236 206 212 183

fish
false
0
Polygon -1 true false 44 131 21 87 15 86 0 120 15 150 0 180 13 214 20 212 45 166
Polygon -1 true false 135 195 119 235 95 218 76 210 46 204 60 165
Polygon -1 true false 75 45 83 77 71 103 86 114 166 78 135 60
Polygon -7500403 true true 30 136 151 77 226 81 280 119 292 146 292 160 287 170 270 195 195 210 151 212 30 166
Circle -16777216 true false 215 106 30

flag
false
0
Rectangle -7500403 true true 60 15 75 300
Polygon -7500403 true true 90 150 270 90 90 30
Line -7500403 true 75 135 90 135
Line -7500403 true 75 45 90 45

flower
false
0
Polygon -10899396 true false 135 120 165 165 180 210 180 240 150 300 165 300 195 240 195 195 165 135
Circle -7500403 true true 85 132 38
Circle -7500403 true true 130 147 38
Circle -7500403 true true 192 85 38
Circle -7500403 true true 85 40 38
Circle -7500403 true true 177 40 38
Circle -7500403 true true 177 132 38
Circle -7500403 true true 70 85 38
Circle -7500403 true true 130 25 38
Circle -7500403 true true 96 51 108
Circle -16777216 true false 113 68 74
Polygon -10899396 true false 189 233 219 188 249 173 279 188 234 218
Polygon -10899396 true false 180 255 150 210 105 210 75 240 135 240

ghost
false
0
Polygon -7500403 true true 30 165 13 164 -2 149 0 135 -2 119 0 105 15 75 30 75 58 104 43 119 43 134 58 134 73 134 88 104 73 44 78 14 103 -1 193 -1 223 29 208 89 208 119 238 134 253 119 240 105 238 89 240 75 255 60 270 60 283 74 300 90 298 104 298 119 300 135 285 135 285 150 268 164 238 179 208 164 208 194 238 209 253 224 268 239 268 269 238 299 178 299 148 284 103 269 58 284 43 299 58 269 103 254 148 254 193 254 163 239 118 209 88 179 73 179 58 164
Line -16777216 false 189 253 215 253
Circle -16777216 true false 102 30 30
Polygon -16777216 true false 165 120 135 120 120 105 135 90 165 90 180 105
Circle -16777216 true false 160 30 30
Rectangle -13345367 true false 129 149 189 209
Polygon -16777216 true false 72 135 135 150 135 152 79 120
Polygon -16777216 true false 187 149 224 127 210 120
Rectangle -1 true false 143 159 173 174

house
false
0
Rectangle -7500403 true true 45 120 255 285
Rectangle -16777216 true false 120 210 180 285
Polygon -7500403 true true 15 120 150 15 285 120
Line -16777216 false 30 120 270 120

leaf
false
0
Polygon -7500403 true true 150 210 135 195 120 210 60 210 30 195 60 180 60 165 15 135 30 120 15 105 40 104 45 90 60 90 90 105 105 120 120 120 105 60 120 60 135 30 150 15 165 30 180 60 195 60 180 120 195 120 210 105 240 90 255 90 263 104 285 105 270 120 285 135 240 165 240 180 270 195 240 210 180 210 165 195
Polygon -7500403 true true 135 195 135 240 120 255 105 255 105 285 135 285 165 240 165 195

line
true
0
Line -7500403 true 150 0 150 300

line half
true
0
Line -7500403 true 150 0 150 150

participant
false
0
Circle -7500403 true true 120 0 60
Polygon -7500403 true true 120 60 120 150 90 240 105 240 135 240 150 180 165 240 195 240 210 240 180 150 180 60
Rectangle -7500403 true true 127 79 172 94
Polygon -7500403 true true 180 60 225 120 210 150 150 75
Polygon -7500403 true true 120 60 75 120 90 150 150 75
Rectangle -13345367 true false 134 74 170 119
Rectangle -1 true false 141 79 163 95
Polygon -16777216 true false 124 61 128 55 140 73 126 62
Polygon -16777216 true false 172 59 179 61 165 74

pentagon
false
0
Polygon -7500403 true true 150 15 15 120 60 285 240 285 285 120

plant
false
0
Rectangle -7500403 true true 135 90 165 300
Polygon -7500403 true true 135 255 90 210 45 195 75 255 135 285
Polygon -7500403 true true 165 255 210 210 255 195 225 255 165 285
Polygon -7500403 true true 135 180 90 135 45 120 75 180 135 210
Polygon -7500403 true true 165 180 165 210 225 180 255 120 210 135
Polygon -7500403 true true 135 105 90 60 45 45 75 105 135 135
Polygon -7500403 true true 165 105 165 135 225 105 255 45 210 60
Polygon -7500403 true true 135 90 120 45 150 15 180 45 165 90

sheep
false
15
Circle -1 true true 203 65 88
Circle -1 true true 70 65 162
Circle -1 true true 150 105 120
Polygon -7500403 true false 218 120 240 165 255 165 278 120
Circle -7500403 true false 214 72 67
Rectangle -1 true true 164 223 179 298
Polygon -1 true true 45 285 30 285 30 240 15 195 45 210
Circle -1 true true 3 83 150
Rectangle -1 true true 65 221 80 296
Polygon -1 true true 195 285 210 285 210 240 240 210 195 210
Polygon -7500403 true false 276 85 285 105 302 99 294 83
Polygon -7500403 true false 219 85 210 105 193 99 201 83

square
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 false false 30 30 270 270

square 2
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 true false 60 60 240 240

star
false
0
Polygon -7500403 true true 151 1 185 108 298 108 207 175 242 282 151 216 59 282 94 175 3 108 116 108

touristspot
false
0
Circle -7500403 true true 60 0 180
Circle -16777216 true false 90 30 120
Circle -7500403 true true 120 60 60

tree
false
0
Circle -7500403 true true 118 3 94
Rectangle -6459832 true false 120 195 180 300
Circle -7500403 true true 65 21 108
Circle -7500403 true true 116 41 127
Circle -7500403 true true 45 90 120
Circle -7500403 true true 104 74 152

triangle
false
0
Polygon -7500403 true true 150 30 15 255 285 255

triangle 2
false
0
Polygon -7500403 true true 150 30 15 255 285 255
Polygon -16777216 true false 151 99 225 223 75 224

truck
false
0
Rectangle -7500403 true true 4 45 195 187
Polygon -7500403 true true 296 193 296 150 259 134 244 104 208 104 207 194
Rectangle -1 true false 195 60 195 105
Polygon -16777216 true false 238 112 252 141 219 141 218 112
Circle -16777216 true false 234 174 42
Rectangle -7500403 true true 181 185 214 194
Circle -16777216 true false 144 174 42
Circle -16777216 true false 24 174 42
Circle -7500403 false true 24 174 42
Circle -7500403 false true 144 174 42
Circle -7500403 false true 234 174 42

turtle
true
0
Polygon -10899396 true false 215 204 240 233 246 254 228 266 215 252 193 210
Polygon -10899396 true false 195 90 225 75 245 75 260 89 269 108 261 124 240 105 225 105 210 105
Polygon -10899396 true false 105 90 75 75 55 75 40 89 31 108 39 124 60 105 75 105 90 105
Polygon -10899396 true false 132 85 134 64 107 51 108 17 150 2 192 18 192 52 169 65 172 87
Polygon -10899396 true false 85 204 60 233 54 254 72 266 85 252 107 210
Polygon -7500403 true true 119 75 179 75 209 101 224 135 220 225 175 261 128 261 81 224 74 135 88 99

wheel
false
0
Circle -7500403 true true 3 3 294
Circle -16777216 true false 30 30 240
Line -7500403 true 150 285 150 15
Line -7500403 true 15 150 285 150
Circle -7500403 true true 120 120 60
Line -7500403 true 216 40 79 269
Line -7500403 true 40 84 269 221
Line -7500403 true 40 216 269 79
Line -7500403 true 84 40 221 269

wolf
false
0
Polygon -16777216 true false 253 133 245 131 245 133
Polygon -7500403 true true 2 194 13 197 30 191 38 193 38 205 20 226 20 257 27 265 38 266 40 260 31 253 31 230 60 206 68 198 75 209 66 228 65 243 82 261 84 268 100 267 103 261 77 239 79 231 100 207 98 196 119 201 143 202 160 195 166 210 172 213 173 238 167 251 160 248 154 265 169 264 178 247 186 240 198 260 200 271 217 271 219 262 207 258 195 230 192 198 210 184 227 164 242 144 259 145 284 151 277 141 293 140 299 134 297 127 273 119 270 105
Polygon -7500403 true true -1 195 14 180 36 166 40 153 53 140 82 131 134 133 159 126 188 115 227 108 236 102 238 98 268 86 269 92 281 87 269 103 269 113

x
false
0
Polygon -7500403 true true 270 75 225 30 30 225 75 270
Polygon -7500403 true true 30 75 75 30 270 225 225 270

@#$#@#$#@
NetLogo 5.3.1
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
default
0.0
-0.2 0 0.0 1.0
0.0 1 1.0 0.0
0.2 0 0.0 1.0
link direction
true
0
Line -7500403 true 150 150 90 180
Line -7500403 true 150 150 210 180

@#$#@#$#@
1
@#$#@#$#@
