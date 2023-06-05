#include <iostream>
#include <string>
#include <thread>
#include <curl/curl.h>

using namespace std;

char errorBuf[CURL_ERROR_SIZE];
char contents[CURL_MAX_WRITE_SIZE];
std::string url;
size_t received_len = 0;

size_t write_func( char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t len = size*nmemb;
	received_len += len;
	return len;
}

int main()
{
	curl_global_init(CURL_GLOBAL_ALL);
	CURL * handle = curl_easy_init();
	CURLcode result;

	cout << "Enter URL:";
	cin >> url;

	while(true)
	{
		received_len = 0;

		// Set options before each call to curl_easy_perform()
		curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
		curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errorBuf);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_func);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, contents);

		// Invoke the URL
		if((result = curl_easy_perform(handle)) == 0)
			cout << "[" << time(0) << "]: \"" << url << "\" Received " << received_len << " bytes ok!\n";
		else
			std::cout << "[" << time(0) << "]: \"" << url << "\" **ERROR: " << errorBuf << " [Error Code: " << result << "]\n";

		// Pause until 5 seconds has passed
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	// NOTE: This will never be called
	curl_easy_cleanup(handle);

	return 0;
}
