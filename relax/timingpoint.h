#pragma once

#include <string>
#include <vector>

#include "parser.h"

class TimingPoint {
	int time;
	double beatLength;
	bool inherited;

public:
	TimingPoint();
	TimingPoint(std::string hObj);
	int getTiming() { return time; }
	double getBeatLength() { return beatLength; }
	bool isInherited() { return inherited; }
};
