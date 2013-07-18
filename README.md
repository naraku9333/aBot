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
    - [x] Write multiple logs, numbered ie: chatlog_<simple_date>.txt
- [x] Keep user log (users who enter channel) and keep list of join+part combos --Not currently written to disk
- [ ] Automatically reconnect to server and channel
- [ ] receive commands from channel and privmsg
- [ ] results sent same manner as received (ie: if command from privmsg answer should be a privmsg)
- [ ] implement command !lastseen [username] --get date + time last time [username] logged on
- [x] implement command !tell [public|private] [user] [message] --send [message] to [username] when they log back in, 
      sent to channel if public or via privmsg if private -- NOTE: All params required!
- [x] implement command !quit -- quits current channel
- [x] implement command !join [channel] --quits current channel and joins specified channel
- [x] implement command !help --print help message to channel
- [ ] OPTIONAL implement command !weather [zip] --display basic weather info
- [ ] OPTIONAL implement command !cowsay --display a cow saying a fortune  http://en.wikipedia.org/wiki/Cowsay


####Building:


####Issues:

- [ ] Logging does not handle redirects (logs list intended channel not the acctual channel)
- [ ] Not curenntly handling any exceptions

 
####Thanks:
To [Thumperrr](https://github.com/Thumperrr) for the use of his regex (and his idea of encapsulating irc messages)
