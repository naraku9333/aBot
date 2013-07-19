#ifndef SV_BOT_HPP
#define SV_BOT_HPP

#include <string>
#include <boost/asio.hpp>
#include <map>
#include <tuple>
#include <list>
//#include "Message.hpp"

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
		void quit();
		void listen();
		void handler(const std::string&);

	private:
		void save_log();
		void help();
		//sv::Message parse_message(const std::string&);//TODO
		Bot() : io(), sock(io){}
		Bot(const Bot&) : io(), sock(io){}

		std::string nick,
			server,
			port,
			channel;

		boost::asio::io_service io;
		boost::asio::ip::tcp::socket sock;
		boost::system::error_code error;

		std::vector<std::string> chat_log;

		typedef std::pair<std::string, std::string> strpair;
		typedef std::tuple<std::string, std::string, bool> relay_data;
		//user last seen <user, list<join date+time, part date+time>>
		std::map< std::string, std::list<strpair>> user_log;
		//<user, list<sender, msg>>
		std::map< std::string, std::list<relay_data>> msg_relay;

		static const int MAX_LOG_FILES = 10;
		static const int MAX_LOG_LENGTH = 100;
	};	
};
#endif
