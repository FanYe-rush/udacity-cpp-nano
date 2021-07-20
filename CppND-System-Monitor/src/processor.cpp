#include "processor.h"

void Processor::Update(const vector<unsigned long> newStat) {
  prev_user_ = user_;
  prev_nice_ = nice_;
  prev_system_ = system_;
  prev_idle_ = idle_;
  prev_iowait_ = iowait_;
  prev_irq_ = irq_;
  prev_softirq_ = softirq_;
  prev_steal_ = steal_;
  prev_guest_ = guest_;
  prev_guest_nice_ = guest_nice_;

  user_ = newStat[0];
  nice_ = newStat[1];
  system_ = newStat[2];
  idle_ = newStat[3];
  iowait_ = newStat[4];
  irq_ = newStat[5];
  softirq_ = newStat[6];
  steal_ = newStat[7];
  guest_ = newStat[8];
  guest_nice_ = newStat[9];
}

float Processor::Utilization() {
  unsigned long prevIdle = prev_idle_ + prev_iowait_;
  unsigned long prevNonIdle = prev_user_ + prev_nice_ + prev_system_ +
                              prev_irq_ + prev_softirq_ + prev_steal_;
  unsigned long prevTotal = prevIdle + prevNonIdle;

  unsigned long idle = idle_ + iowait_;
  unsigned long nonIdle = user_ + nice_ + system_ + irq_ + softirq_ + steal_;
  unsigned long total = idle + nonIdle;

  unsigned long totalD = total - prevTotal;
  unsigned long nonIdleD = nonIdle - prevNonIdle;

  return (float)nonIdleD / totalD;
}