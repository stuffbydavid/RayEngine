#pragma once

#include <functional>
#include <algorithm>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <map>

using namespace std;
using namespace placeholders;

typedef unsigned int uint;
typedef unsigned char uchar;

inline long mod(long a, long b) {
	return (a % b + b) % b;
}

inline float clamp(float x, float mi, float ma) {
	return min(ma, max(mi, x));
}

inline float frand() {
	return (float)rand() / RAND_MAX;
}

inline float frand(float a, float b) {
	return a + frand() * (b - a);
}

inline string to_string_prec(float val, int prec) {

	stringstream ss;
	ss << setprecision(prec) << val;
	return ss.str();

}

inline string date() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H.%M.%S", &tstruct);
	return buf;
}