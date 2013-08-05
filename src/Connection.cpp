#include <Connection.hpp>
sv::Connection::Connection( const std::string& s, const std::string& p) 
			: io(), sock(io), server(s), port(p)
		{		
		}

void sv::Connection::connect()
{
	boost::asio::ip::tcp::resolver resolver(io);
	boost::asio::ip::tcp::resolver::query query(server, port);
	boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

	boost::asio::connect(sock, it);
}

void sv::Connection::send(const std::string& msg)
{
	if(!msg.empty())
		sock.send(boost::asio::buffer(msg + "\r\n"));
}

std::string sv::Connection::receive()
{
	std::array<char, 256> buf;

	size_t len = sock.read_some(boost::asio::buffer(&buf, 256), error);

	if (error == boost::asio::error::eof)
		throw "Connection closed cleanly by peer.";
	else if (error)
			throw boost::system::system_error(error); 

	return std::string(buf.begin(), buf.begin() + len);	
}