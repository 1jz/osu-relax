#pragma once

#include <Windows.h>

#include "TimingPoint.h"
#include "HitObject.h"

class Action {
	int type;
	int time;
	int duration;
	char key;
	INPUT input;
	bool pressed; // true = key down, false = key up

public:
	Action();
	Action(HitObject ho, char key, bool pressed);
	Action(double sliderMultiplier, double beatLength, TimingPoint tp, HitObject ho, char key, bool pressed);
	int getType() { return type; }
	int getTime() { return time; }
	int getDuration() { return duration; }
	char getKey() { return key; }
	bool getPressed() { return pressed; }
	INPUT getInput() { return input; }
	void setTime(int time) { this->time = time; };
	void setDuration(int duration) { this->duration = duration; };
	void setInput(bool pressed);
};

std::vector<Action> parseMap(std::string filename);
