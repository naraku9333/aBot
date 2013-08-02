//
// aBot
// ~~~~~~~~~~
//
// Sean Vogel svogel at comcast dot net
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include "Bot.hpp"
#include <iostream>
#include <boost/thread.hpp>

int main(int argc, char* argv[])
{
	if(argc >= 3)		
	{
		std::string channel = (argc >= 4) ? argv[3] : "##c++";
		std::string port = (argc >= 5) ? argv[4] : "6667";
		
		sv::Bot bot(argv[1], argv[2], port);
		bool connected(false);
		//while(!connected)
		{
			try
			{
				//API key from http://www.wunderground.com/weather/api/
				//REQUIRED to use weather feature
				bot.set_api_key("ac549751ac6f3c73");
				bot.connect();
				bot.join(channel);
				bot.listen();
			}catch(std::exception& e)
			{
				std::cout << e.what() << std::endl;
				/*std::cout << "Connection failed, attempting again in 5 seconds" << std::endl;
				boost::this_thread::sleep_for(boost::chrono::seconds(5));*/
			}

		}	
	}
	else
		std::cout << "USAGE:\nRun with:  abot <nick> <server> <channel>optional <port>optional" << std::endl;
}
