#include <Message.hpp>
#include <iterator>
#include <algorithm>

sv::Message::Message(const std::string& msg) //: sender(), command(), params(), data()
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
    {
        sender = "SERVER";
        data = msg;
    }

	std::stringstream ss(data);
	
    std::copy(std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>(), std::back_inserter(botcommands));
}

bool sv::Message::find_command(const std::string com)
{
	if(!botcommands.empty())
	{
		if(botcommands[0] == com)
			return true;
	}
	return false;
}

std::vector<std::string> sv::Message::command_params()
{		
	return std::vector<std::string>(botcommands.begin() + 1, botcommands.end());
}