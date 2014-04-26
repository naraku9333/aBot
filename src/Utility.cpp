#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <Utility.hpp>
std::string sv::util::time_string()
{
	std::chrono::system_clock::time_point n = std::chrono::system_clock::now();
	time_t t(std::chrono::system_clock::to_time_t(n));
	std::string s(ctime(&t));
	return s.substr(0, s.length() - 3);//remove trailing crlf
}

std::string get_http_data(const std::string& server, const std::string& file)
{
    try
    {
        boost::asio::ip::tcp::iostream sock(server, "http");
        sock.expires_from_now(boost::posix_time::seconds(60));

        if (!sock){ throw "Unable to connect: " + sock.error().message(); }

        // ask for the file
        sock << "GET /" << file << " HTTP/1.0\r\n" << "Host: "
            << server << "\r\n" << "Accept: */*\r\n" << "Connection: close\r\n\r\n";

        // Check that response is OK.
        std::string http_version;
        unsigned int status_code;
        sock >> http_version >> status_code;

        std::string status_message;
        std::getline(sock, status_message);
        if (!sock && http_version.substr(0, 5) != "HTTP/"){ throw "Invalid response\n"; }
        if (status_code != 200){ throw "Response returned with status code " + status_code + status_message; }

        // Process the response headers, which are terminated by a blank line.
        std::string header;
        while (std::getline(sock, header) && header != "\r"){}

        std::string tmp;
        std::getline(sock, tmp, '\r');
        return tmp;// std::string(tmp.begin(), tmp.end() - 2); //strip trailing \r\n
    }
    catch (std::exception& e)
    {
        return e.what();
    }
}

	
void sv::util::html_decode(std::string& str)
{
	using namespace boost::algorithm;
	replace_all(str, "&amp;", "&");
	replace_all(str,  "&quot;", "\"");
	replace_all(str, "&apos;", "\'");
	replace_all(str, "&lt;", "<");
	replace_all(str, "&agtmp;", ">");
	replace_all(str, "&nbsp;", " ");
}

//requires api key from http://www.wunderground.com/weather/api/
std::string sv::util::weather(const std::string& loc, const std::string& api_key)
{
	using boost::property_tree::ptree;
	ptree data;
	try
	{
        if (!api_key.empty())
        {
            std::string raw = util::get_http_data("api.wunderground.com", "/api/" + api_key + "/geolookup/conditions/q/" + loc + ".xml");

            read_xml(std::istringstream(raw), data);
            std::string full_loc, weather, temp, humidity, wind, dewpoint, feel;
            for (auto& a : data.get_child("response.current_observation"))
            {
                if (a.first == "display_location")full_loc = a.second.get<std::string>("full");
                if (a.first == "weather")weather = a.second.data();
                if (a.first == "temperature_string")temp = a.second.data();
                if (a.first == "relative_humidity")humidity = a.second.data();
                if (a.first == "wind_string")wind = a.second.data();
                if (a.first == "dewpoint_string")dewpoint = a.second.data();
                if (a.first == "feelslike_string")feel = a.second.data();
            }
            return "Current conditions for " + full_loc + " is " + weather + " with temperature of "
                + temp + " with a relative humidity of " + humidity + " Dewpoint is " + dewpoint + " and it feels like " + feel
                + " with wind " + wind;
        }
        else
            throw "API key required";
	}
	catch(std::exception& e)
	{
		return e.what();
	}
}

const std::vector<std::string> sv::util::cow_data()
{
	std::string res = util::get_http_data("www.iheartquotes.com", "/api/v1/random?max_lines=4");
	util::html_decode(res);
	boost::algorithm::replace_all(res, "\r\n", " ");
	boost::algorithm::replace_all(res, "\n", " ");

	std::istringstream oss(res);
	std::string t, line;
	std::vector<std::string> data;

	const int width = 60;
	while(oss >> t)
	{
		if(t.size() + line.size() < width)
		{
			line += " " + t;			
		}
		else
		{
			data.push_back(line);
			line = t;
		}		
	}
	if(!t.empty())
			data.push_back(t);
	
	return data;		
}
