#include "classping.h"
#include <time.h>
#include <iomanip>
#include <vector>

ClassPing::ClassPing()
{

}

ClassPing::~ClassPing(){
	if(thr.joinable()){
		stopThread();
	}
}

std::string ClassPing::ssdp_discover(){
	char buffer[128];
	std::string result = "";
	FILE* pfile = popen("gssdp-discover -i enp2s0 --timeout=3", "r");
//	FILE* pfile = popen("gssdp-discover -i wlp3s0 --timeout=3", "r");
	while(fgets(buffer, sizeof buffer, pfile)){
		result = result + buffer;
	}
	pclose(pfile);
	return result;
}

size_t ClassPing::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

nlohmann::json ClassPing::device_func (std::string res){
	std::istringstream file(res);
	std::string line;
	std::string cikan = "";
	std::string location = "Location:";
	while(std::getline(file, line)){

		if(line.find(location) !=std::string::npos){
			std::string xml = "xml";
			line.substr(0, line.find(xml));
			cikan = line.erase(0, location.length() + 3);
		}
		else{

		}

		std::string xmlStr;
		CURL* curl = curl_easy_init();
		if(curl){
			curl_easy_setopt(curl, CURLOPT_URL, cikan.c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuf);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &xmlStr);

			CURLcode result = curl_easy_perform(curl);
			if(result == CURLE_OK){
				pugi::xml_document doc;
				if (!doc.load_string(xmlStr.c_str())) {
					std::cerr << "Failed to load xml file\n";
					return nlohmann::json::object();
				}
				nlohmann::json j;
				pugi::xml_node xmlNode = doc.document_element();
				for(pugi::xml_node node = xmlNode.first_child(); node; node = node.next_sibling()){
					std::string tag = node.name();
					std::string value = node.text().get();

					if(node.first_child()){
						j[tag] = nlohmann::json::object();
						for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()){
							std::string child_tag = child.name();
							std::string child_value = child.text().get();
							j[tag][child_tag] = child_value;
						}
					}
					else{
						j[tag] = value;
					}
				}

				struct_ping p;
				curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &p.host);

				auto obj = nlohmann::json::object();
				obj["modelName"] = j["device"]["modelName"];
				obj["presentationURL"] = j["device"]["presentationURL"];
				obj["ip"] = p.host;

				std::string h(p.host);
				device_map.insert({h, obj});

				for(auto& i : device_map){
					device_arr.push_back(i.second);

					char buffer[128];
					std::string result = "";
					FILE* pfile = popen((std::string("ping -c 1 ")+p.host).c_str(), "r");
					while(fgets(buffer, sizeof buffer, pfile)){
						result = result + buffer;
					}
					pclose(pfile);

					std::istringstream p_file(result);
					std::string p_line;

					std::string rtt = "rtt";
					std::string equal = "= ";
					std::string ms = "ms";
					std::string slash = "/";

					while(std::getline(p_file, p_line)){
						if(p_line.find(rtt) != std::string::npos){
							std::string value = p_line.substr(0, p_line.find(equal));
							p_line.erase(0, value.length() + equal.length());

							p.min = p_line.substr(0, p_line.find(slash));
							p_line.erase(0, p.min.length() + slash.length());

							p.avg = p_line.substr(0, p_line.find(slash));
							p_line.erase(0, p.avg.length() + slash.length());

							p.max = p_line.substr(0, p_line.find(slash));
							p_line.erase(0, p.max.length() + slash.length());

							p.mdev = p_line.substr(0, p_line.find(ms));
						}
					}
					ping_map[i.first] = p;
				}
			}
			else{
			}
		}
		curl_easy_cleanup(curl);
	}
}

bool ClassPing::thread_func(){
	while(stop_bool){
		{
			std::unique_lock <std::mutex> lock_ping(mutex_ping);
			cv.wait_for(lock_ping, std::chrono::milliseconds(1000));
			if(!stop_bool){
				break;
			}
		}
		std::string res = "";
		res = ssdp_discover();
		device_func(res);
	}
	return stop_bool;
}

void ClassPing::runThread(){
	thr = std::thread(&ClassPing::thread_func, this);
}

void ClassPing::stopThread(){
	stop_bool = false;
	cv.notify_one();
	thr.join();
}
