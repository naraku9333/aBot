#ifndef SV_BOT_HPP
#define SV_BOT_HPP

#include <string>
#include <Message.hpp>
#include <Connection.hpp>
#include <Logger.hpp>
#include <boost/noncopyable.hpp>
#include <atomic>

namespace sv
{
	class Bot : boost::noncopyable
	{
	public:
		Bot() = delete;
		Bot(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
		~Bot();

		void connect();//
		void join(const std::string&);
		void quit();
		void listen();

	private:
		void handler(Message);
		void send(const std::string&);
		void send_priv(const std::string&, const std::string&);
		std::string receive();
		void help();	
		void handle_bot_commands(Message&);
		void check_messages(const std::string&);
		void send_cow();

		std::string nick,
			channel, server;
		Connection connection;
		bool exit;
		log::Logger log;
		std::string api_key;

		////user last seen <user, list<join date+time, part date+time>>
		//std::map< std::string, std::list<std::pair<std::string, std::string>>> user_log;

		//<user, list<sender, msg>>
		std::map< std::string, std::list
			<std::tuple<std::string, std::string, bool>>> 
			msg_relay;		

        //bot owner - me!
        std::string owner;
	};	
};
#endif
