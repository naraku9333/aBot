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
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
	if(argc >= 3)		
	{
		std::string channel = (argc >= 4) ? argv[3] : "#abot_test";
		std::string port = (argc >= 5) ? argv[4] : "6667";
		
		//API key from http://www.wunderground.com/weather/api/
		//REQUIRED to use weather feature
		std::string api = (argc >= 6) ? argv[5] : "";

        std::string owner = (argc >= 7) ? argv[6] : "";

		sv::Bot bot(argv[1], argv[2], port, api, owner);
		
		try
		{
			using namespace std::chrono;

			bot.connect();
			std::thread t(&sv::Bot::listen, std::ref(bot));

            std::this_thread::sleep_for(seconds(10));
           
			bot.join(channel);
			t.join();
		}
		catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}	
	else
		std::cout << "USAGE:\nRun with:  abot <nick> <server> <channel>optional <port>optional "
                  << "<weather api key> optional <owner>optional" << std::endl;
}
