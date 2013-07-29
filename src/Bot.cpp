#include "Bot.hpp"
#include <iostream>
#include <chrono>
#include <boost/filesystem.hpp>
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <fstream>
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
	if(!api_key.empty())
		send("PRIVMSG " + channel + " :-- !weather <zip> | <city> <state|province|country *2 letter abbreviation> get current weather conditions");
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
	else if(com.find_command(std::string("!weather")) && !v.empty())
	{
		std::string s = (v.size() > 1) ? v[1] + "/" + v[0] : v[0];		
		send("PRIVMSG " + channel + " :" + weather(s));
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

//from http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp03/iostreams/http_client.cpp
//requires api key from http://www.wunderground.com/weather/api/
std::string sv::Bot::weather(std::string& loc)
{
	if(!api_key.empty())
	{
		try
		{
			boost::asio::ip::tcp::iostream s;
			s.expires_from_now(boost::posix_time::seconds(60));

			// Establish a connection to the server.
			s.connect("api.wunderground.com", "http");
			if (!s)
			{
			  std::cout << "Unable to connect: " << s.error().message() << "\n";
			  return std::string();
			}

			// ask for the xml file
			s << "GET " << "/api/" + api_key + "/geolookup/conditions/q/" + loc + ".xml" << " HTTP/1.0\r\n";
			s << "Host: " << "api.wunderground.com" << "\r\n";
			s << "Accept: */*\r\n";
			s << "Connection: close\r\n\r\n";

			// Check that response is OK.
			std::string http_version;
			s >> http_version;
			unsigned int status_code;
			s >> status_code;
			std::string status_message;
			std::getline(s, status_message);
			if (!s || http_version.substr(0, 5) != "HTTP/")
			{
			  std::cout << "Invalid response\n";
			  return std::string();
			}
			if (status_code != 200)
			{
			  std::cout << "Response returned with status code " << status_code << "\n";
			  return std::string();
			}

			// Process the response headers, which are terminated by a blank line.
			std::string header;
			while (std::getline(s, header) && header != "\r")
			  std::cout << header << "\n";
			std::cout << "\n";

			// Write the remaining data to output.
			std::stringstream ss;
			ss << s.rdbuf();

			using boost::property_tree::ptree;
			ptree data;
			read_xml(ss, data);
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
		catch (std::exception& e)
		{
			std::cout << "Exception: " << e.what() << "\n";
		}
	}
}

void sv::Bot::set_api_key(const std::string& key)
{
	api_key = key;
}
