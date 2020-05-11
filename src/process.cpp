#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

//Return this process's ID
int Process::Pid() { return pid_; }

//Return this process's CPU utilization
float Process::CpuUtilization() { 
  long Active_Jiffies = LinuxParser::ActiveJiffies( pid_ ) / sysconf(_SC_CLK_TCK);
  long int TotalJiffies = Process::UpTime();
  cpu_utilization_ = float(Active_Jiffies) / float(TotalJiffies);
  return cpu_utilization_;
}

//Return the command that generated this process
string Process::Command() { return  LinuxParser::Command( pid_ ); }

//Return this process's memory utilization
string Process::Ram() { 
  ram_ = std::stoi(LinuxParser::Ram( pid_ ));
  return LinuxParser::Ram( pid_ ) ; 
}

//Return the user (name) that generated this process
string Process::User() { return LinuxParser::User( pid_ ) ; }

//Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime( pid_ ); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return this->cpu_utilization_ >  a.cpu_utilization_;  //sort by CPU Utilization
  //return this->ram_ > a.ram_;                            //sort by RAM Does not work properly but why?
}