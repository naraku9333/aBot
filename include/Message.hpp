#ifndef SV_BOT_MESSAGE_HPP
#define SV_BOT_MESSAGE_HPP
/**
	IRCBOT Message class
	-encapsulate message parts
	-parse messages

	Idea borrowed from Thumperrr
*/
#include <string>
#include <boost/regex.hpp>
#include <sstream>
#include <vector>

namespace sv
{
	class Message
	{		
	public:
		std::string sender, command, params, data;
		std::vector<std::string> botcommands;

		explicit Message(const std::string& msg) //: sender(), command(), params(), data()
		{
			static const boost::regex regex("^(:(\\S+ ))?(\\S+ )((?!:).+?)?(:(.+))?(\r\n)?$");
			boost::smatch match;
			if(boost::regex_match(msg, match, regex))
			{
				sender  = match[2];
				sender = (sender.find('!') != std::string::npos && (sender.substr(0, sender.find('!'))).length() > 0) ? 
						sender.substr(0, sender.find('!')) ://I dont want host info
						"SERVER";
		
				command = match[3];
				if(command.size() > 0) command.pop_back();
				params  = match[4];
				data    = match[6];
			}
			else
				sender = "SERVER";

			std::stringstream ss(data);
			std::string  tempstr;
			while(ss >> tempstr)
				botcommands.push_back(tempstr);
		}

		bool find_command(std::string& com)
		{
			if(!botcommands.empty())
			{
				if(botcommands[0] == com)
					return true;
			}
			return false;
		}

		std::vector<std::string> command_params()
		{		
			return std::vector<std::string>(botcommands.begin() + 1, botcommands.end());
		}
	};
}
#endif
