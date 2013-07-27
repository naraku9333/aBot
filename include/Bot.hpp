#ifndef SV_BOT_HPP
#define SV_BOT_HPP

#include <string>
#include <map>
#include <tuple>
#include <list>
#include "Message.hpp"
#include "Connection.hpp"

namespace sv
{
	class Bot
	{
	public:
		Bot(const std::string&, const std::string&, const std::string&);
		~Bot();

		void connect();
		void join(const std::string&);
		void quit();
		void listen();

	private:
		void handler(Message&);
		void send(const std::string&);
		std::string receive();
		void save_log();
		void help();
		void handle_bot_commands(Message&);
		void check_messages(const std::string&);

		//workaround for VS
		Bot() : connection("","")/*= delete*/{}
		Bot(const Bot&) : connection("","")/*= delete*/{}

		std::string nick,
			channel;
		Connection connection;
		bool exit;

		std::vector<std::string> chat_log;

		typedef std::pair<std::string, std::string> strpair;

		//user last seen <user, list<join date+time, part date+time>>
		std::map< std::string, std::list<strpair>> user_log;

		//<user, list<sender, msg>>
		std::map< std::string, std::list
			<std::tuple<std::string, std::string, bool>>> 
			msg_relay;

		static const int MAX_LOG_FILES = 10;
		static const int MAX_LOG_LENGTH = 3;
	};	
};
#endif
