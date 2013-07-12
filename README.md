aBot
====

An IRC bot to be entered into a cplusplus.com monthly 
community competition http://cppcomp.netne.net/showthread.php?tid=4


####Planned features:

 - [ ] Log channel activity -messages, join + parts, and possibly global notices
   - [ ] Store logs in std::vector<>
     - [ ] Set max size, if met write log to file
 - [ ] Write multiple logs, numbered ie: log_01.txt to log_xx.txt
 - [ ] Keep user log (users who enter channel) and keep list of join+part combos
 - [ ] Automatically reconnect to server and channel
 - [ ] receive commands from channel and privmsg
   - [ ] results sent same manner as received (ie: if command from privmsg answer should be a privmsg)
 - [ ] implement lastseen [username] --get date + time last time [username] logged on
 - [ ] implement !saywha? [username] [numposts] --show [usernames]'s last [numposts] messages
 - [ ] implement !passiton [username] [message] --send [message] to [username] when they log back in
 - [ ] OPTIONAL implement !weather [zip] --display basic weather info
 - [ ] OPTIONAL implement cowsay http://en.wikipedia.org/wiki/Cowsay --display a cow saying a fortune