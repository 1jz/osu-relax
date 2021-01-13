#pragma once

#include <string>
#include <vector>

#include "parser.h"

enum HitObjectType {
	Circle = 0b0001,
	Slider = 0b0010,
	Spinner = 0b1000
};

class HitObject {
	int time;
	int type;
	int sliderSlides;
	double sliderLength;
	int spinnerEndTime;

public:
	HitObject();
	HitObject(std::string hObj);
	int getTiming() { return time; }
	int getType() { return type; }
	int getSliderSlides() { return sliderSlides; }
	double getSliderLength() { return sliderLength; }
	int getSpinnerEndTime() { return spinnerEndTime; }
};
