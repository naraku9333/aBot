#include "Bot.hpp"
#include <iostream>
#include <boost/filesystem.hpp>
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <fstream>
#include "Message.hpp"
#include <algorithm>
#include "Utility.hpp"
#include <sstream>
#include <string>

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

void sv::Bot::send_priv(const std::string& msg , const std::string& to)
{
	connection.send("PRIVMSG " + to + " :" + msg);
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

void sv::Bot::handler(Message& msg)
{
	if(msg.sender != "SERVER")
	{
		strpair s;

		std::string now = util::time_string();
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
		while(m.size() >= MAX_LOG_FILES)
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
	send_priv("aBot - An IRC bot to be entered into a cplusplus.com monthly community competition http://cppcomp.netne.net/showthread.php?tid=4", channel);
	send_priv("-- !join <channel>  join the specified channel", channel);
	send_priv("-- !quit  quit current channel\n", channel);
	send_priv("-- !exit  shutdown " + nick + "\n", channel);
	send_priv("-- !tell <private|public> <nick> <message>  Send <message> to <nick> in channel if public or via privmsg if private.", channel);
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
		send_priv( weather(s), channel);
	}
	else if(com.find_command(std::string(("!cowsay"))))
	{
		cowsay();
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

//requires api key from http://www.wunderground.com/weather/api/
std::string sv::Bot::weather(const std::string& loc) const
{
	if(!api_key.empty())
	{	
		using boost::property_tree::ptree;
		ptree data;
		std::string raw = util::get_http_data("api.wunderground.com", "/api/" + api_key + "/geolookup/conditions/q/" + loc + ".xml");
		read_xml(raw, data);
		std::string full_loc, weather, temp, humidity, wind, dewpoint, feel;
		for(auto& a : data.get_child("response.current_observation"))
		{
			if(a.first == "display_location")full_loc = a.second.get<std::string>("full");
			if(a.first == "weather")weather = a.second.data();
			if(a.first == "temperature_string")temp = a.second.data();
			if(a.first == "relative_humidity")humidity = a.second.data();
			if(a.first == "wind_string")wind = a.second.data();
			if(a.first == "dewpoint_string")dewpoint = a.second.data();
			if(a.first == "feelslike_string")feel = a.second.data();
		}
		return "Current conditions for " + full_loc + " is " + weather + " with temperature of " 
			+ temp + " with a relative humidity of " + humidity + " Dewpoint is " + dewpoint + " and it feels like " + feel
			+ " with wind " + wind;		
	}
}

void sv::Bot::set_api_key(const std::string& key)
{
	api_key = key;
}

void sv::Bot::cowsay()
{
	std::string res = util::get_http_data("www.iheartquotes.com", "/api/v1/random?max_lines=4");
	util::html_decode(res);
	boost::algorithm::replace_all(res, "\r\n", " ");
	boost::algorithm::replace_all(res, "\n", " ");

	std::istringstream oss(res);
	std::string t, line;
	std::vector<std::string> data;

	const int width = 60;
	while(oss >> t)
	{
		if(t.size() + line.size() < width)
		{
			line += " " + t;			
		}
		else
		{
			data.push_back(line);
			line = t;
		}		
	}
	if(!t.empty())
			data.push_back(t);
	
	send_cow(data, width);		
}


void sv::Bot::send_cow(const std::vector<std::string>& data, const int w)
{
	const int pad = 2;
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