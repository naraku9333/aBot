#include "Bot.hpp"
#include <cstdlib>
#include <iostream>
#include <array>
#include <chrono>
#include <boost/filesystem.hpp>
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <fstream>
#include <sstream>
#include "Message.hpp"

namespace sv
{
	std::string time_string()
	{
		std::chrono::system_clock::time_point n = std::chrono::system_clock::now();
		time_t t(std::chrono::system_clock::to_time_t(n));
		std::string s(ctime(&t));
		return s.substr(0, s.length() - 3);//remove trailing crlf
	}
}

sv::Bot::Bot(const std::string& n, const std::string& s, const std::string& p = std::string("6667"))
	: nick(n), connection(s,p), exit(false)
{
	
}

sv::Bot::~Bot()
{
	if(!chat_log.empty())
		save_log();
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

std::string sv::Bot::receive()
{
	return connection.receive();
}

void sv::Bot::join(const std::string& chan)
{
	if(!chan.empty())
	{
		channel = chan;
		send("JOIN " + channel);
	}
}

void sv::Bot::quit()
{
	if(!channel.empty())
	{
		send("PART " + channel);
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

void sv::Bot::handler(Message& msg)
{
	if(msg.sender != "SERVER")
	{
		strpair s;

		std::string now = time_string();
		std::string newmsg = now + "\t<" + msg.sender +">";

		if(msg.command == "JOIN")
		{
			s.first = now;
			user_log[msg.sender].push_back(s);
 			newmsg += " has joined " + channel;
			check_messages(msg.sender);
		}
		else if(msg.command == "PART" || msg.command == "QUIT")
		{
			if(user_log.find(msg.sender) != user_log.end() && !user_log[msg.sender].empty())
				user_log[msg.sender].back().second = now;
			
			newmsg += " has quit " + channel;
		}
		if(msg.command == "PRIVMSG")
		{
			handle_bot_commands(msg);
		}
		if(user_log[msg.sender].size() > MAX_LOG_FILES)
			user_log[msg.sender].pop_front();
			
		newmsg += "  " + msg.data;
		std::cout << newmsg << std::endl;
		chat_log.push_back(newmsg);

		if(chat_log.size() >= MAX_LOG_LENGTH)
		{
			save_log();
		}
	}
}

void sv::Bot::save_log()
{
	using namespace boost::filesystem;
	path p("log/chat/");
	directory_iterator endit;

	std::map<time_t, path> m;

	if(exists(p) && is_directory(p))
	{
		for(directory_iterator dit(p); dit != endit; ++dit)
			m.insert(std::make_pair(last_write_time(*dit), *dit));

		//trim number of log files
		while(m.size() > MAX_LOG_FILES)
		{
			auto it = m.begin();
			remove(it->second);
			m.erase(it);
		}		
	}
	else
		create_directories(p);

	using namespace boost::gregorian;
	date d(day_clock::local_day());

	std::string filename = p.string() + "chatlog_" + channel + "_" + to_iso_string(d) + ".txt";
	std::ofstream of;

	if(exists(path(filename)))
		of.open(filename, std::ios::app);
	else

		of.open(filename);

	for(auto& a : chat_log)
		of << a << std::endl;

	chat_log.clear();
}

void sv::Bot::help()
{
	send("PRIVMSG " + channel + " :aBot - An IRC bot to be entered into a cplusplus.com monthly community"
		+ " competition http://cppcomp.netne.net/showthread.php?tid=4");
	send("PRIVMSG " + channel + " :-- !join <channel>  join the specified channel");
	send("PRIVMSG " + channel + " :-- !quit  quit current channel\n");
	send("PRIVMSG " + channel + " :-- !exit  shutdown " + nick + "\n");
	send("PRIVMSG " + channel + " :-- !tell <private|public> <nick> <message>  Send <message> to <nick> in channel if "
			+ "public or via privmsg if private.");
}

void sv::Bot::handle_bot_commands(Message& com)
{	
	std::vector<std::string> v = com.command_params();

	if(com.find_command(std::string(("!quit"))))
	{
		send("PART " + channel);
		channel.clear();
	}
	else if(com.find_command(std::string("!join")) && v.size() > 0)
	{
		
		if(!channel.empty())
			quit();
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
					send("PRIVMSG " + user + " :While you were out " + std::get<0>(a) +" said: " + std::get<1>(a));
				else
					send("PRIVMSG " + channel + " :@" + user + " While you were out " + std::get<0>(a) +" said: " + std::get<1>(a));
							
			}
			msg_relay[user].clear();
		}
	}
}
