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

int main(int argc, char* argv[])
{
	if(argc >= 3)		
	{
		std::string port = (argc >= 5) ? argv[4] : "6667";
				
		sv::Bot bot(argv[1], argv[2], port);
		bot.connect();

		std::string channel = (argc >= 4) ? argv[3] : "##c++";
		bot.join(channel);

		bot.listen();
	}
	else
		std::cout << "USAGE:\nRun with:  abot <nick> <server> <channel>optional <port>optional" << std::endl;
}
