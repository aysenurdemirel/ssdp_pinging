#include "classping.h"
#include <iomanip>

int main()
{
	ClassPing obj_p;
	obj_p.runThread();

	while(1){
		std::cout << "\033[2J\033[1;1H";

		std::cout << std::setw(6) << std::right << "IP"
				  << std::setw(16) << std::right << "Min (ms)"
				  << std::setw(12) << std::right << "Avg (ms)"
				  << std::setw(12) << std::right << "Max (ms)"
				  << std::setw(12) << std::right << "Mdev (ms)" << std::endl;

		std::cout << std::setw(6) << std::right << "-----------"
				  << std::setw(11) << std::right << "--------"
				  << std::setw(12) << std::right << "--------"
				  << std::setw(12) << std::right << "--------"
				  << std::setw(12) << std::right << "---------" << std::endl;


		for(auto& i: obj_p.ping_map){
			std::cout << std::setw(10) << std::left << i.first;
			std::cout << std::setw(10) << std::right << i.second.min;
			std::cout << std::setw(13) << std::right << i.second.avg;
			std::cout << std::setw(12) << std::right << i.second.max;
			std::cout << std::setw(12) << std::right << i.second.mdev << std::endl;
		}

		std::cout << '\r';
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}
