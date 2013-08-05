#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <map>
#include <Logger.hpp>
#include <Utility.hpp>

void sv::log::Logger::save_log(const std::string& channel)
{
	using namespace boost::filesystem;
	path p("log/chat/");

	std::map<time_t, path> m;

	if(exists(p) && is_directory(p))
	{
		directory_iterator endit;
		for(directory_iterator dit(p); dit != endit; ++dit)
			m.insert(std::make_pair(last_write_time(*dit), *dit));

		//trim number of log files
		while(m.size() >= MAX_LOG_FILES)
		{
			auto it = m.begin();
			remove(it->second);
			m.erase(it);
		}		
	}
	else
		create_directories(p);

	using namespace boost::gregorian;
	date d(day_clock::local_day());

	std::string filename = p.string() + "chatlog_" + channel + "_" + to_iso_string(d) + ".txt";
	std::ofstream of;

	if(exists(path(filename)))
		of.open(filename, std::ios::app);
	else
		of.open(filename);

	for(auto& a : chat_log)
		of << a << std::endl;

	chat_log.clear();
}

void sv::log::Logger::log_message(const std::string& usr, const std::string& channel, std::string msg)
{
	msg = util::time_string() + "\t<" + usr +">\t" + msg;
	chat_log.push_back(msg);

	if(chat_log.size() >= MAX_LOG_LENGTH)
	{
		save_log(channel);
	}
}

