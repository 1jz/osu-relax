#include "parser.h"

// Timing point string follows the following format:
// time,beatLength,meter,sampleSet,sampleIndex,volume,uninherited,effects
// https://github.com/ppy/osu-wiki/blob/master/wiki/osu!_File_Formats/Osu_(file_format)/en.md#timing-points
void parseHitObject(std::vector<std::string>& vec, std::string hitObjLine) {
	int delimPos = 0;
	while ((delimPos = hitObjLine.find(",")) != std::string::npos) {
		vec.push_back(hitObjLine.substr(0, delimPos));
		hitObjLine.erase(0, delimPos + 1);
	}

	if (!hitObjLine.empty()) {
		vec.push_back(hitObjLine);
	}
}
