#ifndef SV_BOT_CONNECTION_HPP
#define SV_BOT_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <array>
#include <string>
/**
* IRCBOT Connection class
* -encapsulate connection detials
*	
*/
namespace sv
{
	class Connection : boost::noncopyable
	{
		boost::asio::io_service io;
		boost::asio::ip::tcp::socket sock;
		boost::system::error_code error;

		std::string server, port;
		Connection() : io(), sock(io) /*= delete*/{}
	
	public:
		/**
		* Create connection to server on port
		*
		* @param server s to connect to
		* @param port p to connect over
		*/
		Connection( const std::string& s, const std::string& p);

		void connect();

		void send(const std::string& msg);

		std::string receive();
	};
}
#endif
