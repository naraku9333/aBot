#ifndef SV_BOT_HPP
#define SV_BOT_HPP

#include <string>
#include <boost/asio.hpp>
#include <functional>
#include <map>
#include <memory>

namespace sv
{
	class Bot
	{
	public:
		Bot(const std::string& n, const std::string& s, const std::string& p);
		~Bot();

		void connect();
		void send(const std::string& msg);
		std::string receive();
		void join(const std::string& channel);
		void listen();

	private:
		Bot() : io(), sock(io){}
		Bot(const Bot&) : io(), sock(io){}

		std::string nick,
			server,
			port,
			channel;

		boost::asio::io_service io;
		boost::asio::ip::tcp::socket sock;
		boost::system::error_code error;
		std::map<std::string, std::function<void(void)>> task_map;
	};	
};
#endif