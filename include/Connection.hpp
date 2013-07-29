#ifndef SV_BOT_CONNECTION_HPP
#define SV_BOT_CONNECTION_HPP
#include <boost/asio.hpp>
#include <array>
#include <string>
/**
	IRCBOT Connection class
	-encapsulate connection detials
	
*/
class Connection
{
	boost::asio::io_service io;
	boost::asio::ip::tcp::socket sock;
	boost::system::error_code error;

	std::string server, port;
	Connection(const Connection&) : io(), sock(io) /*= delete*/{}
	Connection() : io(), sock(io) /*= delete*/{}
	
public:
	Connection( const std::string& s, const std::string& p) 
		: io(), sock(io), server(s), port(p)
	{		
	}

	void connect()
	{
		boost::asio::ip::tcp::resolver resolver(io);
		boost::asio::ip::tcp::resolver::query query(server, port);
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

		boost::asio::connect(sock, it);
	}

	void send(const std::string& msg)
	{
		if(!msg.empty())
			sock.send(boost::asio::buffer(msg + "\r\n"));
	}

	std::string receive()
	{
		std::array<char, 256> buf;

		size_t len = sock.read_some(boost::asio::buffer(&buf, 256), error);

		if (error == boost::asio::error::eof)
			throw "Connection closed cleanly by peer.";
		else if (error)
				throw boost::system::system_error(error); 

		return std::string(buf.begin(), buf.begin() + len);	
	}
};
#endif
