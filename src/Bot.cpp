#include "Bot.hpp"
#include <cstdlib>
#include <iostream>
#include <array>
#include <thread>
#include <algorithm>

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
			send("USER " + nick + " nowhere" + " none" + " :Ima Robot");//CHANGE THIS!!!
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
	/*std::array<char, 256> buf;
	size_t len = sock.read_some(boost::asio::buffer(buf, 256), error);

	return std::string(buf.begin(), buf.begin() + len);*/
	std::string s(256, '\0');
	size_t len = sock.read_some(boost::asio::buffer(&s[0], 256), error);
	s.resize(len);
	return s;
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

		else if(s.find("hi " + nick) != std::string::npos)
		{
			std::string u = s.substr(s.find(":") + 1, s.find("!") - 1);
			send("PRIVMSG " + channel + " :how ya doin " + u);
		}

		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.

		else if (error)
			throw boost::system::system_error(error); // Some other error.

		std::cout << s << std::endl;
	}	
}