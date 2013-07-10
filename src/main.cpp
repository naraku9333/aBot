//
// aBot
// ~~~~~~~~~~
//
// Sean Vogel svogel at comcast dot net
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


//#include "Bot.hpp"
//
//int main()
//{
//	sv::Bot bot("aBot", "irc.freenode.net", "6667");
//	bot.connect();
//	bot.join("#ecc_comp_sci");
//	bot.listen();
//}
#include <iostream>
using namespace std;

int main()
{
    cout << "Enter\n";
    char s[20];
    cin.get(s, 10);
    char c = cin.peek();
    cout << c << '\n';
    cout << "The state is " << cin.rdstate() << '\n';
    cin.ignore(11, '\n');
    cout << "The state is " << cin.rdstate() << '\n';
    char e = cin.peek();
    cout << "The state is " << cin.rdstate() << '\n';
    cout << e <<'\n';
    return 0;
}