#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

void System::Update() {
  LinuxParser::ParseStat();
  LinuxParser::ParseMeminfo();
}

Processor& System::Cpu() {
  vector<unsigned long> newStat = LinuxParser::CpuUtilization();
  cpu_.Update(newStat);
  return cpu_;
}

vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  int count = pids.size();
  processes_.clear();

  for (int i = 0; i < count; i++) {
    processes_.emplace_back(Process(pids[i]));
  }

  sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

std::string System::Kernel() {
  if ("" == kernel) {
    kernel = LinuxParser::Kernel();
  }
  return kernel;
}

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {
  if ("" == os) {
    os = LinuxParser::OperatingSystem();
  }
  return os;
}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
