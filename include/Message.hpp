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

		explicit Message(const std::string& msg);

		bool find_command(std::string& com);

		std::vector<std::string> command_params();
	};
}
#endif
