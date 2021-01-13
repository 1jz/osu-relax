#include "hitobject.h"

HitObject::HitObject() {
	time = -1;
	type = -1;
	sliderSlides = -1;
	sliderLength = -1;
	spinnerEndTime = -1;
}

HitObject::HitObject(std::string hObj) {
	std::vector<std::string> vec;
	parseHitObject(vec, hObj);

	time = std::stoi(vec[2]);
	type = std::stoi(vec[3]);

	if (type & HitObjectType::Circle) {
		sliderSlides = -1;
		sliderLength = -1;
		spinnerEndTime = -1;
	}
	else if (type & HitObjectType::Slider) {
		sliderSlides = std::stoi(vec[6]);
		sliderLength = std::stod(vec[7]);
		spinnerEndTime = -1;
	}
	else if (type & HitObjectType::Spinner) {
		sliderSlides = -1;
		sliderLength = -1;
		spinnerEndTime = std::stoi(vec[5]) - time;
	}
}
