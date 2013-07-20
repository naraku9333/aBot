#include "Bot.hpp"
#include <cstdlib>
#include <iostream>
#include <array>
#include <thread>
#include <chrono>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <fstream>
#include <sstream>

sv::Bot::Bot(const std::string& n, const std::string& s, 
			 const std::string& p = std::string("6667"))
	: nick(n), server(s), port(p), io(), sock(io)
{
	
}

sv::Bot::~Bot()
{
	if(!chat_log.empty())
		save_log();
}

void sv::Bot::connect()
{
	try
	{
		boost::asio::ip::tcp::resolver resolver(io);
		boost::asio::ip::tcp::resolver::query query(server, port);
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

		boost::asio::connect(sock, it);

		send("NICK " + nick);
		send("USER guest 0 * :Ima Robot");
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	int count = 0;
}

void sv::Bot::send(const std::string& msg)
{
	if(!msg.empty())
		sock.send(boost::asio::buffer(msg + "\r\n"));
}

std::string sv::Bot::receive()
{
	std::array<char, 256> buf;

	size_t len = sock.read_some(boost::asio::buffer(&buf, 256), error);

	if (error == boost::asio::error::eof)
		throw "Connection closed cleanly by peer.";
	else if (error)
			throw boost::system::system_error(error); 

	return std::string(buf.begin(), buf.begin() + len);	
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
	while(true)
	{
		std::string s = receive();

		if(s.find("PING") != std::string::npos)
			send("PONG");

		else
			handler(s);	
	}	
}

void sv::Bot::handler(const std::string& msg)
{
	//std::cout << msg << std::endl;

	static const boost::regex regex("^(:(\\S+ ))?(\\S+ )((?!:).+?)?(:(.+))?(\r\n)?$");
    boost::smatch match;
	std::string sender, command, params, data;
    if(boost::regex_match(msg, match, regex))
    {
        sender  = match[2];
		sender = (sender.find('!') != std::string::npos) ? 
				sender.substr(0, sender.find('!')) ://I dont want host info
				"SERVER";
		
        command = match[3];
		if(command.size() > 0) command.pop_back();
        params  = match[4];
        data    = match[6];
		
		if(sender != "SERVER")
		{
			strpair s;

			std::chrono::system_clock::time_point n = std::chrono::system_clock::now();
			time_t t(std::chrono::system_clock::to_time_t(n));
			std::string now = ctime(&t);
			now = now.substr(0, now.length() - 3);//remove trailing crlf

			std::string newmsg = now + "\t<" + sender +">";

			if(command == "JOIN")
			{
				s.first = now;
				user_log[sender].push_back(s);
 				newmsg += " has joined " + channel;

				if(msg_relay.find(sender) != msg_relay.end())
				{
					if(!msg_relay[sender].empty())
					{
						for(const auto& a : msg_relay[sender])
						{
							if(std::get<2>(a))
								send("PRIVMSG " + sender + " :While you were out " + std::get<0>(a) +" said: " + std::get<1>(a));
							else
								send("PRIVMSG " + channel + " :@" + sender + " While you were out " + std::get<0>(a) +" said: " + std::get<1>(a));
							
						}
						msg_relay[sender].clear();
					}
				}
			}
			else if(command == "PART" || command == "QUIT")
			{
				if(user_log.find(sender) != user_log.end() && !user_log[sender].empty())
					user_log[sender].back().second = now;
			
				newmsg += " has quit " + channel;
			}
			if(command == "PRIVMSG")
			{
				std::vector<std::string> botcommands;
				std::stringstream ss(data);
				std::string  tempstr;
				while(ss >> tempstr)
					botcommands.push_back(tempstr);

				if(botcommands[0] == "!quit")
					send("PART " + channel);

				else if(botcommands[0] == "!join" && botcommands.size() > 1)
				{
					quit();
					join(botcommands[1]);
				}
				else if(botcommands[0] == "!tell" && botcommands.size() > 3)
				{
					std::string temp;//(botcommands.size(),0);
					std::for_each(botcommands.begin() + 3, botcommands.end(),[&](std::string s){temp += " " + s;});
					bool b = false;
					if(botcommands[1] == "private")
						b = true;
					msg_relay[botcommands[2]].push_back(std::make_tuple(sender, temp, b));
				}
				else if(botcommands[0] == "!help")
				{
					send("PRIVMSG " + channel + " :aBot - An IRC bot to be entered into a cplusplus.com monthly community"
						+ " competition http://cppcomp.netne.net/showthread.php?tid=4");
					send("PRIVMSG " + channel + " :-- !join <channel>  join the specified channel");
					send("PRIVMSG " + channel + " :-- !quit  quit current channel\n");
					send("PRIVMSG " + channel + " :-- !tell <private|public> <nick> <message>  Send <message> to <nick> in channel if "
						 + "public or via privmsg if private.");
				}
			}
			if(user_log[sender].size() > MAX_LOG_FILES)
				user_log[sender].pop_front();
			
			newmsg += "\t" + data;
			std::cout << newmsg << std::endl;
			chat_log.push_back(newmsg);

			if(chat_log.size() >= MAX_LOG_LENGTH)
			{
				save_log();
			}
		}
	}
    else //throw std::runtime_error("Unable to match IRC message to regular expression.");
		std::cout << msg << std::endl;//probably a server message
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
