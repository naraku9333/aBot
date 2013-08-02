#ifndef SV_BOT_UTILITY_HPP
#define SV_BOT_UTILITY_HPP
#include <iostream>
#include <string>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

namespace sv
{
	namespace util
	{
		std::string time_string()
		{
			std::chrono::system_clock::time_point n = std::chrono::system_clock::now();
			time_t t(std::chrono::system_clock::to_time_t(n));
			std::string s(ctime(&t));
			return s.substr(0, s.length() - 3);//remove trailing crlf
		}

		//from http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp03/iostreams/http_client.cpp
		std::string get_http_data(const std::string& server, const std::string& file)
		{
			try
			{
				boost::asio::ip::tcp::iostream s;
				s.expires_from_now(boost::posix_time::seconds(60));

				// Establish a connection to the server.
				s.connect(server, "http");
				if (!s)
				{
					std::cout << "Unable to connect: " << s.error().message() << "\n";
					return std::string();
				}

				// ask for the file
				s << "GET " << file << " HTTP/1.0\r\n";
				s << "Host: " << server << "\r\n";
				s << "Accept: */*\r\n";
				s << "Connection: close\r\n\r\n";

				// Check that response is OK.
				std::string http_version;
				s >> http_version;
				unsigned int status_code;
				s >> status_code;
				std::string status_message;
				std::getline(s, status_message);
				if (!s || http_version.substr(0, 5) != "HTTP/")
				{
					std::cout << "Invalid response\n";
					return std::string();
				}
				if (status_code != 200)
				{
					std::cout << "Response returned with status code " << status_code << "\n";
					return std::string();
				}

				// Process the response headers, which are terminated by a blank line.
				std::string header;
				while (std::getline(s, header) && header != "\r")
					std::cout << header << "\n";
				std::cout << "\n";

				// Write the remaining data to output.
				std::stringstream ss;
				ss << s.rdbuf();
				return ss.str();
			}
			catch(std::exception& e)
			{
				std::cout << e.what() << std::endl;
				return std::string();
			}
		}

		void html_decode(std::string& str)
		{
			using namespace boost::algorithm;
			replace_all(str, "&amp;", "&");
			replace_all(str,  "&quot;", "\"");
			replace_all(str, "&apos;", "\'");
			replace_all(str, "&lt;", "<");
			replace_all(str, "&agtmp;", ">");
			replace_all(str, "&nbsp;", " ");
		}
	}	
}
#endif