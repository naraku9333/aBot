#include <Bot.hpp>
#include <iostream>
#include <fstream>
#include <Message.hpp>
#include <algorithm>
#include <Utility.hpp>
#include <sstream>
#include <string>

sv::Bot::Bot(const std::string& n, const std::string& s,
			 const std::string& p = std::string("6667"), const std::string& api = "")
			 : nick(n), connection(s,p), exit(false), api_key(api)
{
	
}

sv::Bot::~Bot()
{
		log.save_log(channel);
}

void sv::Bot::connect()
{	
	connection.connect();

	send("NICK " + nick);
	send("USER guest 0 * :Ima Robot");	
}

void sv::Bot::send(const std::string& msg)
{
	connection.send(msg);
}

void sv::Bot::send_priv(const std::string& msg , const std::string& to)
{
	connection.send("PRIVMSG " + to + " :" + msg);
	if(to == channel)
		log.log_message(nick, to, msg);
}

std::string sv::Bot::receive()
{
	return connection.receive();
}

void sv::Bot::join(const std::string& chan)
{
	if(!chan.empty())
	{
		quit();
		channel = chan;
		send("JOIN " + channel);
	}
}

void sv::Bot::quit()
{
	if(!channel.empty())
	{
		send("PART " + channel);
		log.save_log(channel);
		channel.clear();
	}
}

void sv::Bot::listen()
{	
	while(!exit)
	{
		std::string s = receive();

		if(s.find("PING") != std::string::npos)
			send("PONG");

		else
			handler(Message(s));	
	}	
}

void sv::Bot::handler(Message msg)
{
	if(msg.sender != "SERVER")
	{
		std::string newmsg;

		if(msg.command == "JOIN")
		{
 			newmsg += "has joined " + channel;
			check_messages(msg.sender);
		}
		else if(msg.command == "PART" || msg.command == "QUIT")
		{						
			newmsg += "has quit " + channel;
		}
		//TODO handle other commands ie: NICK, ACTION, etc..

		newmsg += "  " + msg.data;
		std::cout << msg.sender << channel << newmsg << std::endl;//dbg
		log.log_message(msg.sender, channel, newmsg);
		if(msg.command == "PRIVMSG")
		{
			handle_bot_commands(msg);
		}
	}
}

void sv::Bot::help()
{
	send_priv("aBot - An IRC bot to be entered into a cplusplus.com monthly community competition http://cppcomp.netne.net/showthread.php?tid=4", channel);
	send_priv("-- !join <channel>  join the specified channel", channel);
	send_priv("-- !quit  quit current channel\n", channel);
	send_priv("-- !exit  shutdown " + nick + "\n", channel);
	send_priv("-- !tell <private|public> <nick> <message>  Send <message> to <nick> in channel if public or via privmsg if private.", channel);
	send_priv("-- !cowsay display a fortune told by a cow", channel);
	if(!api_key.empty())
		send_priv("-- !weather <zip> | <city> <state|province|country> get current weather conditions", channel);
}

void sv::Bot::handle_bot_commands(Message& com)
{	
	std::vector<std::string> v = com.command_params();

	if(com.find_command(std::string(("!quit"))))
	{
		quit();
	}
	else if(com.find_command(std::string("!join")) && v.size() > 0)
	{				
		join(v[0]);		
	}
	else if(com.find_command(std::string("!tell")) && v.size() > 2)
	{
		std::string temp;//(botcommands.size(),0);
		std::for_each(v.begin() + 2, v.end(),[&](std::string s){temp += " " + s;});
		bool b = false;
		if(v[0] == "private")
			b = true;
		msg_relay[v[1]].push_back(std::make_tuple(com.sender, temp, b));
	}
	else if(com.find_command(std::string("!help")))
	{
		help();
	}
	else if(com.find_command(std::string(("!exit"))))
	{
		exit = true;
	}
	else if(com.find_command(std::string("!weather")) && !v.empty())
	{
		std::string s = (v.size() > 1) ? v[1] + "/" + v[0] : v[0];		
		send_priv( util::weather(s, api_key), channel);
	}
	else if(com.find_command(std::string(("!cowsay"))))
	{
		send_cow();
	}
}

void sv::Bot::check_messages(const std::string& user)
{
	if(msg_relay.find(user) != msg_relay.end())
	{
		if(!msg_relay[user].empty())
		{
			for(const auto& a : msg_relay[user])
			{
				if(std::get<2>(a))
					send_priv("While you were out " + std::get<0>(a) +" said: " + std::get<1>(a), user);
				else
					send_priv("@" + user + " While you were out " + std::get<0>(a) +" said: " + std::get<1>(a), channel);
							
			}
			msg_relay[user].clear();
		}
	}
}

void sv::Bot::send_cow()
{
	const int pad = 2;
	const int w = 60;
	auto data = util::cow_data();
	send_priv(" " + std::string(w + pad,'_'), channel);
	for(auto& a : data)
	{		
		send_priv("| " + a + ((a.size() < w) ? std::string(w - a.size(), ' ') : std::string()) + "|", channel);
	}
	send_priv(" " + std::string(w + pad,'-'), channel);
	send_priv("     \\  ^__^", channel);
	send_priv("      \\ (oo)\\_______", channel);
	send_priv("        (__)\\       )\\/\\", channel);
	send_priv("             ||----w |", channel);
	send_priv("             ||     ||", channel);
}
