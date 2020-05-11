#include <string>

#include "format.h"

using std::string;

// helper function
string getTime(long time) {
    int hours;
    int minutes;
    int seconds;
    hours = time/3600;
    time = time%3600;
    minutes = time/60;
    seconds = time%60;
    return std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
};

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
  return getTime(seconds); 
}