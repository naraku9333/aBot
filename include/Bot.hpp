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
		Bot(const std::string&, const std::string&, const std::string&);
		~Bot();

		void connect();
		void send(const std::string&);
		std::string receive();
		void join(const std::string&);
		void listen();
		void handler(const std::string&);
	private:
		void write_log();

		Bot() : io(), sock(io){}
		Bot(const Bot&) : io(), sock(io){}

		std::string nick,
			server,
			port,
			channel;

		boost::asio::io_service io;
		boost::asio::ip::tcp::socket sock;
		boost::system::error_code error;

		std::map<std::string, std::function<void(void)>> task_map;//?
		std::vector<std::string> chat_log;

		typedef std::pair<std::string, std::string> session;
		//user last seen <user, list<join date+time, part date+time>>
		std::map< std::string, std::list<session>> user_log;
		static const int MAX_LOGS = 10;
	};	
};
#endif