## README

The files in the source folder are NOT to be used for analysis.

Note that all server badges have ids in the 2000s
          all part badges have ids in the 1000s

All of the csv files in this folder are just lists of duples: (badgeIDa, badgeIDb)\n.

‘all_interactions.csv’ includes all interactions between badges. That is, every recorded interaction from every consenting part badge and every server badge. Note that this means that there will be _many_ duplicates because of unsuccessful comms and because every check-in / interaction will be recorded twice.

‘part_interactions.csv’ includes _only_ the data that was recorded on individual badges. Note, that this means that technically, this data should be a complete picture of the network, since it still includes ‘check-ins’ because the part badges recorded that interaction.

‘server_interactions.csv’ is just a list of all check-ins.

‘nw_template.nlogo’ is a NetLogo model I made that takes care of reading in data from a CSV file, creating agents for each badge, and creating links by the read in interactions. It also loads in the NW extension and calculates a few NW stats as a demo.

The stuff in the source folder is the origin data (and the python script) that I used to generate those CSVs. You can ignore that.