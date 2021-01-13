#include "timingpoint.h"

TimingPoint::TimingPoint() {
	time = -1;
	beatLength = -1;
	inherited = false;
}

TimingPoint::TimingPoint(std::string hObj) {
	std::vector<std::string> vec;
	parseHitObject(vec, hObj);

	time = std::stoi(vec[0]);
	beatLength = std::stod(vec[1]);
	inherited = std::stoi(vec[6]);
}
