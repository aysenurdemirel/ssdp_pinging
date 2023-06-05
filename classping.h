#ifndef CLASSPING_H
#define CLASSPING_H

#include <iostream>
#include <sstream>
#include <cstring>
#include <curl/curl.h>
#define PUGIXML_HEADER_ONLY
#include "pugixml.hpp"
#include "json.hpp"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

struct struct_ping{
	std::string min = "";
	std::string avg = "";
	std::string max = "";
	std::string mdev = "";
	char* host = NULL;
};

class ClassPing
{
public:
	ClassPing();
	~ClassPing();
	std::string ssdp_discover();
	std::string ping_c();
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
	nlohmann::json device_func (std::string res);
	bool thread_func();
	void runThread();
	void stopThread();

	nlohmann::json device_arr = nlohmann::json::array();
	std::map <std::string, struct_ping> ping_map;

private:
	char errorBuf[CURL_ERROR_SIZE];
	std::map <std::string, nlohmann::json> device_map;

	std::thread thr;
	std::mutex mutex_ping;
	std::condition_variable cv;
	bool stop_bool = true;
};

#endif // CLASSPING_H
