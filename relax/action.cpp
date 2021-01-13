#include "action.h"
#include "error.h"

Action::Action() {
	type = -1;
	time = -1;
	duration = -1;
	key = '\0';
	input = { {} };
	pressed = false;
}

Action::Action(HitObject ho, char key, bool pressed) {
	time = ho.getTiming();
	type = ho.getType();
	duration = ho.getSpinnerEndTime();
	if (!pressed) {
		if(type & HitObjectType::Circle) {
			time += 4; // random offset number
		}
		else {
			time += duration;
		}
	}
	this->key = key;
	setInput(pressed);
}

Action::Action(double sliderMultiplier, double beatLength, TimingPoint tp, HitObject ho, char key, bool pressed) {
	time = ho.getTiming();
	type = ho.getType();
	double duration  = ho.getSliderLength() / (sliderMultiplier * 100) * (beatLength * (tp.isInherited() ? 1.0 : std::abs(tp.getBeatLength()) / 100));
	this->duration = duration * ho.getSliderSlides();
	if (!pressed) {
		time += this->duration;
	}
	this->key = key;
	setInput(pressed);
}

void Action::setInput(bool pressed) {
	this->pressed = pressed;
	input = { {} };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(0));
	input.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);
}

std::vector<Action> parseMap(std::string filename) {
	std::cout << "Reading [" << filename << "]..." << std::endl;
	std::ifstream file(filename);

	if (!file) {
		throw Error(4, std::string("Cannot open [" + filename + "]"));
	}

	// 0001 = TimingPoints, 0010 = HitObjects, 0100 = Difficulty
	int section = 0b000;

	std::vector<TimingPoint> timingPoints;
	std::vector<HitObject> hitObjects;

	double sliderMultiplier = 0;

	std::string line;

	while (std::getline(file, line)) {
		if (!strcmp(line.c_str(), "")) {
			section = 0b000;
		}
		else if (line.compare("[TimingPoints]") == 0) {
			section = 0b001;
		}
		else if (line.compare("[HitObjects]") == 0) {
			section = 0b010;
		}
		else if (line.compare("[Difficulty]") == 0) {
			section = 0b100;
		}
		else {
			if (section & 0b001) {
				timingPoints.push_back(TimingPoint(line));
			}
			else if (section & 0b010) {
				hitObjects.push_back(HitObject(line));
			}
			else if (section & 0b100) {
				if (line.find("SliderMultiplier:") == 0) {
					sliderMultiplier = std::stod(line.substr(line.find(":") + 1));
					section = 0b000;
				}
			}
		}
	}

	std::vector<Action> actions;
	std::vector<TimingPoint>::iterator currentTP = timingPoints.begin();
	double beatLength = currentTP->getBeatLength();

	// sets current timing point to the first hit object found (for maps with broken timings)
	while (currentTP != (timingPoints.end() - 1) && (currentTP)->getTiming() != hitObjects[0].getTiming()) {
		currentTP++;
		if (currentTP->isInherited()) {
			beatLength = currentTP->getBeatLength();
		}
	}

	char key = OSU_KEY_1;

	for (HitObject ho : hitObjects) {
		// updates latest beatLength and sets current timing point to match timing point at specified hit object (deals with multiple timings that some maps have for some reason..)
		while (currentTP != (timingPoints.end() - 1) && ho.getTiming() >= (currentTP + 1)->getTiming()) {
			currentTP++;
			if (currentTP->isInherited()) {
				beatLength = currentTP->getBeatLength();
			}
		}

		// backup check for beatLength if final timing point has a change
		if (currentTP->isInherited()) {
			beatLength = currentTP->getBeatLength();
		}

#ifdef DEBUG
		std::cout << "BL: " << beatLength <<
			" | HT: " << ho.getTiming() <<
			" | TT: " << currentTP->getTiming() << ", " << currentTP->getBeatLength() << ", " << currentTP->isInherited() << std::endl;
#endif // DEBUG

		if (ho.getType() & HitObjectType::Circle || ho.getType() & HitObjectType::Spinner) {
			actions.push_back(Action(ho, key, true)); // key down action
			actions.push_back(Action(ho, key, false)); // key up action
		}
		else if (ho.getType() & HitObjectType::Slider) {
			actions.push_back(Action(sliderMultiplier, beatLength, *currentTP, ho, key, true)); // key down action
			actions.push_back(Action(sliderMultiplier, beatLength, *currentTP, ho, key, false)); // key up action
		}

		if (key == OSU_KEY_1) {
			key = OSU_KEY_2;
		}
		else {
			key = OSU_KEY_1;
		}
	}

	std::cout << "Done." << std::endl;

	return actions;
}