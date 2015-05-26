#include <Bot.hpp>
#include <iostream>
#include <fstream>
#include <Message.hpp>
#include <algorithm>
#include <Utility.hpp>
#include <sstream>
#include <string>

sv::Bot::Bot(const std::string& n, const std::string& s,
    const std::string& p = "6667", const std::string& api = "", const std::string& o = "")
			 : nick(n), server(s), connection(s,p), exit(false), api_key(api)
{
    if (!o.empty()) owner = o;
}

sv::Bot::~Bot()
{
		log.save_log(channel);
}

void sv::Bot::connect()
{	
	connection.connect();

    send("NICK " + nick);
    send("USER " + nick + " 0 " + server + " :Ima Robot");
}

void sv::Bot::send(const std::string& msg)
{
	connection.send(msg);
	std::cout <<"DBG: " << msg << std::endl;//DBG
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
		std::cout << s << std::endl;//DBG
	
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

		newmsg += "  " + msg.data;
		std::cout << msg.sender << channel << newmsg << std::endl;//dbg
		log.log_message(msg.sender, channel, newmsg);
		if(msg.command == "PRIVMSG")
		{
			handle_bot_commands(msg);
		}
	}
    else
    {        
        auto it = msg.botcommands.begin();
        if ((it = std::find(msg.botcommands.begin(), msg.botcommands.end(), "PING")) != msg.botcommands.end())
        {
            send("PONG " + *++it);
        }
    }
}

void sv::Bot::help()
{
	send_priv("\2aBot - An IRC bot to be entered into a cplusplus.com monthly community competition http://cppcomp.netne.net/showthread.php?tid=4 \2", channel);
	send_priv("-- \2?join <channel>\2  join the specified channel", channel);
	send_priv("-- \2?quit\2  quit current channel\n", channel);
	send_priv("-- \2?exit\2  shutdown " + nick + "\n", channel);
	send_priv("-- \2?tell <private|public> <nick> <message>\2  Send <message> to <nick> in channel if public or via privmsg if private.", channel);
	send_priv("-- \2?cowsay\2 display a fortune told by a cow", channel);
    if (!api_key.empty())
        send_priv("-- \2?weather <zip> | <city> <state|province|country>\2 get current weather conditions", channel);
    
}

void sv::Bot::handle_bot_commands(Message& com)
{	
	std::vector<std::string> v = com.command_params();
   
	if(com.find_command("?quit"))
	{
        if (com.sender == owner)
        {
            quit();
        }
        else send_priv("You are not authorized to run that command!", channel);
	}
	else if(com.find_command("?join") && v.size() > 0)
	{				
        if (com.sender == owner)
        {
            join(v[0]);
        }
        else send_priv("You are not authorized to run that command!", channel);
	}
    else if(com.find_command("?exit"))
	{		
        if (com.sender == owner)
        {
            exit = true;
        }
        else send_priv("You are not authorized to run that command!", channel);
	}
	else if(com.find_command("?tell") && v.size() > 2)
	{
		std::string temp;//(botcommands.size(),0);
		std::for_each(v.begin() + 2, v.end(),[&](std::string s){temp += " " + s;});
		
		bool b = (v[0] == "private");
		msg_relay[v[1]].push_back(std::make_tuple(com.sender, temp, b));
	}
	else if(com.find_command("?help"))
	{
		help();
	}
	
	else if(com.find_command("?weather") && !v.empty())
	{
		std::string s = (v.size() > 1) ? v[1] + "/" + v[0] : v[0];		
		send_priv( util::weather(s, api_key), channel);
	}
	else if(com.find_command("?cowsay"))
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
		send_priv("| " + a + ((a.size() < w) ? std::string(w - a.size(), ' ') : "") + "|", channel);
	}
	send_priv(" " + std::string(w + pad,'-'), channel);
	send_priv("     \\  ^__^", channel);
	send_priv("      \\ (oo)\\_______", channel);
	send_priv("        (__)\\       )\\/\\", channel);
	send_priv("             ||----w |", channel);
	send_priv("             ||     ||", channel);
}
