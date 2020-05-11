#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

//Return the system's CPU
Processor& System::Cpu() { return cpu_; }

//Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // ID's of all processors
  vector<int> pids = LinuxParser::Pids();
  // container for all processors, initialized with their ID's
  for(int pid : pids) {
      processes_.emplace_back(Process(pid));
  }
  // sort processors by the overloaded operator "less than"
  std::sort(processes_.begin(), processes_.end()); 
  // no lambda function needed
  //[](Process& a, Process& b) { return std::stoi(a.Ram()) > std::stoi(b.Ram()); });
  
  return processes_;
}


std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long System::UpTime() { return LinuxParser::UpTime(); }