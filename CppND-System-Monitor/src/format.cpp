#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  unsigned long hour = seconds / 3600;
  unsigned long remain = seconds % 3600;
  unsigned long min = remain / 60;
  remain = remain % 60;
  unsigned long sec = remain;

  char resultBuffer[10];

  sprintf(resultBuffer, "%02lu:%02lu:%02lu", hour, min, sec);

  return resultBuffer;
}