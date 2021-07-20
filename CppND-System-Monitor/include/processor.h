#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

using std::vector;

class Processor {
 public:
  float Utilization();
  void Update(const vector<unsigned long> newStat);

 private:
  unsigned long user_ = 0;
  unsigned long nice_ = 0;
  unsigned long system_ = 0;
  unsigned long idle_ = 0;
  unsigned long iowait_ = 0;
  unsigned long irq_ = 0;
  unsigned long softirq_ = 0;
  unsigned long steal_ = 0;
  unsigned long guest_ = 0;
  unsigned long guest_nice_ = 0;

  unsigned long prev_user_ = 0;
  unsigned long prev_nice_ = 0;
  unsigned long prev_system_ = 0;
  unsigned long prev_idle_ = 0;
  unsigned long prev_iowait_ = 0;
  unsigned long prev_irq_ = 0;
  unsigned long prev_softirq_ = 0;
  unsigned long prev_steal_ = 0;
  unsigned long prev_guest_ = 0;
  unsigned long prev_guest_nice_ = 0;
};

#endif