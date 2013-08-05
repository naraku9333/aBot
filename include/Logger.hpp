#ifndef SV_BOT_LOGGER_HPP
#define SV_BOT_LOGGER_HPP
#include <map>
#include <tuple>
#include <list>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>

namespace sv
{
	namespace log
	{
		class Logger : boost::noncopyable
		{
			
			std::vector<std::string> chat_log;

			typedef std::pair<std::string, std::string> strpair;

			//user last seen <user, list<join date+time, part date+time>>
			std::map< std::string, std::list<strpair>> user_log;

			//<user, list<sender, msg>>
			std::map< std::string, std::list
				<std::tuple<std::string, std::string, bool>>> 
				msg_relay;

			static const int MAX_LOG_FILES = 10;
			static const int MAX_LOG_LENGTH = 100;
			
		public:
			Logger(){}

			void log_message(const std::string&, const std::string&, std::string);

			void save_log(const std::string&);
		};
	}
}
#endif