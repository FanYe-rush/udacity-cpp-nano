#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

LinuxParser::StatCache LinuxParser::statCache = LinuxParser::StatCache();
LinuxParser::MeminfoCache LinuxParser::meminfoCache =
    LinuxParser::MeminfoCache();

void LinuxParser::ParseStat() {
  string line;
  string token;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> token;
      if ("cpu" == token) {
        linestream >> statCache.user >> statCache.nice >> statCache.system >>
            statCache.idle >> statCache.iowait >> statCache.irq >>
            statCache.softirq >> statCache.steal >> statCache.guest >>
            statCache.guest_nice;
      } else if ("processes" == token) {
        linestream >> statCache.processes;
      } else if ("procs_running" == token) {
        linestream >> statCache.procs_running;
      }
      line.clear();
    }
  }
}

void LinuxParser::ParseMeminfo() {
  string line;
  string token;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> token;
      if ("MemTotal:" == token) {
        linestream >> meminfoCache.total;
      } else if ("MemFree:" == token) {
        linestream >> meminfoCache.free;
      } else if ("MemAvailable:" == token) {
        linestream >> meminfoCache.available;
      } else if ("Buffers:" == token) {
        linestream >> meminfoCache.buffer;
      } else if ("Cached:" == token) {
        linestream >> meminfoCache.cached;
      }
      line.clear();
    }
  }
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  unsigned long used = meminfoCache.total - meminfoCache.free;
  unsigned long nonCacheBuffer =
      used - (meminfoCache.buffer + meminfoCache.cached);
  return (nonCacheBuffer * 1.0) / meminfoCache.total;
}

long LinuxParser::UpTime() {
  float systemUptime, idleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> systemUptime >> idleTime;
  }
  return long(systemUptime + idleTime);
}

vector<unsigned long> LinuxParser::CpuUtilization() {
  return {statCache.user,      statCache.nice,   statCache.system,
          statCache.idle,      statCache.iowait, statCache.irq,
          statCache.softirq,   statCache.steal,  statCache.guest,
          statCache.guest_nice};
}

int LinuxParser::TotalProcesses() { return statCache.processes; }

int LinuxParser::RunningProcesses() { return statCache.procs_running; }

string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) +
                       kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmd);
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  long memSize;
  string line, token;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> token;
      if ("VmSize:" == token) {
        linestream >> memSize;
      }
    }
  }
  return to_string(memSize / 1024);
}

string LinuxParser::Uid(int pid) {
  string uid;
  string line, token;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> token;
      if ("Uid:" == token) {
        linestream >> uid;
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) {
  string uid, user, ignored, id;
  string line, token;

  uid = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> ignored >> id;
      if (id == uid) {
        return user;
      }
    }
  }
  return "";
}

long LinuxParser::UpTime(int pid) {
  long ticks;
  string line, token, ignored;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 21; i++) {
        linestream >> ignored;
      }
      linestream >> ticks;
    }
  }
  return LinuxParser::UpTime() - ticks / sysconf(_SC_CLK_TCK);
}

long LinuxParser::TotalCpuTime(int pid) {
  long utimeTick, stimeTick;
  string line, token, ignored;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 13; i++) {
        linestream >> ignored;
      }
      linestream >> utimeTick >> stimeTick;
    }
  }
  return (utimeTick + stimeTick) / sysconf(_SC_CLK_TCK);
}

float LinuxParser::CpuUtilization(int pid) {
  return 1.0 * LinuxParser::TotalCpuTime(pid) / LinuxParser::UpTime(pid);
}
