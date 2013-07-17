aBot
====

An IRC bot to be entered into a cplusplus.com monthly 
[community competition] (http://cppcomp.netne.net/showthread.php?tid=4)

####Dependencies:

    - C++11
    - Boost version 1.35.0 or later http://www.boost.org/
    

####Planned features:

- [x] Log channel activity -messages, join + parts, and possibly global notices
    - [x] Store logs in std::vector<>
    - [x] Set max size, if met write log to file
    - [x] Write multiple logs, numbered ie: log_01.txt to log_xx.txt
- [x] Keep user log (users who enter channel) and keep list of join+part combos --Not currently written to disk
- [ ] Automatically reconnect to server and channel
- [ ] receive commands from channel and privmsg
- [ ] results sent same manner as received (ie: if command from privmsg answer should be a privmsg)
- [ ] implement command !lastseen [username] --get date + time last time [username] logged on
- [ ] implement command !tell [username] [message] --send [message] to [username] when they log back in
- [ ] implement command !quit -- quits current channel
- [ ] implement command !join [channel] --joins channel
- [ ] OPTIONAL implement command !weather [zip] --display basic weather info
- [ ] OPTIONAL implement command !cowsay --display a cow saying a fortune  http://en.wikipedia.org/wiki/Cowsay


####Building:


####Issues:

- [ ] Logging does not handle redirects (logs list intended channel not the acctual channel)
 
####Thanks:
To [Thumperrr](https://github.com/Thumperrr) for the use of his regex (and his idea of encapsulating irc messages)
