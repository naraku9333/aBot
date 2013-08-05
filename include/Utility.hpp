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
		/**
		* Gets the current time as a std::string.
		*
		* @return std::string containing the current time
		*/
		std::string time_string();

		/**
		* Send get request for file from server
		* 
		* @param server ie: "www.google.com"
		* @param file to retrieve ie: "/search?q=irc+bot"
		* @return response data as string
		*
		* from http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/example/cpp03/iostreams/http_client.cpp 
		*/
		std::string get_http_data(const std::string& server, const std::string& file);

		/**
		* Strip html encoding from a std::string.
		*
		* @param The string to strip of encoding
		*/
		void html_decode(std::string& str);

		std::string weather(const std::string&, const std::string&);

		const std::vector<std::string> cow_data();

		void save_log();
	}	
}
#endif