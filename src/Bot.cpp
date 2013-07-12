#include "Bot.hpp"
#include <cstdlib>
#include <iostream>
#include <array>
#include <thread>
#include <algorithm>
#include <chrono>
#include <sstream>

sv::Bot::Bot(const std::string& n, const std::string& s, 
			 const std::string& p = std::string("6667"))
	: nick(n), server(s), port(p), io(), sock(io)
{
	
}

sv::Bot::~Bot()
{
}

void sv::Bot::connect()
{
	try
	{
		boost::asio::ip::tcp::resolver resolver(io);
		boost::asio::ip::tcp::resolver::query query(server, port);
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

		boost::asio::connect(sock, it);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	int count = 0;

	while(true)
	{
		++count;
		
		if(count == 3)
		{
			send("NICK " + nick);
			//USER username host servername :realname
			send("USER " + nick + " 0 * " + ":Ima Robot");
			break;
		}
		//ignore first 3 messages
		std::cout << receive() << std::endl;//DBG
	}
}

void sv::Bot::send(const std::string& msg)
{
	if(!msg.empty())
		sock.send(boost::asio::buffer(msg + "\r\n"));
}

std::string sv::Bot::receive()
{
	std::array<char, 256> buf;
	size_t len = sock.read_some(boost::asio::buffer(buf, 256), error);

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

void sv::Bot::listen()
{	
	while(true)
	{
		std::string s = receive();

		if(s.find("PING") != std::string::npos)
			send("PONG");

		else
			handler(s);
		
		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.

		else if (error)
			throw boost::system::system_error(error); // Some other error.

		//std::cout << s << std::endl;
	}	
}

void sv::Bot::handler(const std::string& msg)
{
	std::cout << msg << std::endl;

	//parse msg
	std::stringstream ss(msg);
	std::string temp;
	ss >> temp;

	if(temp.find("!") != std::string::npos)
	{
		std::string u = temp.substr(temp.find(":") + 1, temp.find("!") - 1);
		//send("PRIVMSG " + channel + " :how ya doin " + u);

		session s;

		std::chrono::system_clock::time_point n = std::chrono::system_clock::now();
		time_t t(std::chrono::system_clock::to_time_t(n));
		std::string now = ctime(&t);

		std::string newmsg = now + "\t" + u;

		ss >> temp;//command
		if(temp == "JOIN")//msg.find("JOIN") != std::string::npos)
		{
			s.first = now;
			user_log[u].push_back(s);
			newmsg += " has joined " + channel;
		}
		else if(temp == "PART" || temp == "QUIT")//(msg.find("PART") != std::string::npos)
		{
			if(user_log.find(u) != user_log.end())
			user_log[u].back().second = now;
			
			newmsg += " has quit " + channel;
		}
		if(user_log[u].size() > MAX_LOGS)
			user_log[u].pop_front();
		
		std::getline(ss, temp);
		if(temp.find(":") != std::string::npos)
		{
			temp = temp.substr(temp.find(":") + 1);
			newmsg += temp;
		}

		chat_log.push_back(newmsg);
	}	
}