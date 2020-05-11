#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read and return the operating system name
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

// Read and return the kernel
string LinuxParser::Kernel() {
  string os, version, kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  int MemTotal, MemFree;
  string name, value, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> value) {
        if (name == "MemTotal") {
          MemTotal = std::stoi(value);
        }
        else if (name == "MemFree") {
          MemFree = std::stoi(value);
        }
      }
    }
    return (MemTotal-MemFree)/float(MemTotal);
  }
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line; 
  string value1;
  string value2;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> value1 >> value2;
  }
  return std::stoi(value1);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}
  

//Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string number;
  vector<string> cpu_info;
  int total_time = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      while (linestream >> number) {
          cpu_info.push_back(number);
      }
      total_time = std::stol(cpu_info[13])+std::stol(cpu_info[14])+std::stol(cpu_info[15])+std::stol(cpu_info[16]);
    }
  return total_time;
}

//Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_info = LinuxParser::CpuUtilization();
  vector<CPUStates> active_CPUstates {kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_};
  long active_jiffies = 0;
  for(auto i: active_CPUstates) {
    active_jiffies += std::stol(cpu_info[i]);
  }
  return active_jiffies; 
}


//Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpu_info = LinuxParser::CpuUtilization();
  vector<CPUStates> idle_CPUstates {kIdle_, kIOwait_};
  long idle_jiffies = 0;
  for(auto i: idle_CPUstates) {
    idle_jiffies += std::stol(cpu_info[i]);
  }
  return idle_jiffies; 
}

//Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string number;
  vector<string> cpu_info;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      while (linestream >> number) {
        if (number != "cpu") {
          cpu_info.push_back(number);
        }
      }
    }
  return cpu_info;
}

//Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line; 
  string name;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> name >> value) {
        if (name == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

//Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line; 
  string name;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> name >> value) {
        if (name == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

//Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
      std::getline(stream, line);
      return line;
    }
  return {};
}

//Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string name, value, line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> name >> value) {
        if (name == "VmSize:") {
          return to_string(std::stoi(value)/1000);
        }
      }
    }
  }
  return {};
}

//Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, id;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> id) {
        if (key == "Uid:") {
          return id;
        }
      }
    }
  }
  return {};
}

//Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string Uid = LinuxParser::Uid(pid);
  string line, user, x, value1, value2;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> user >> x >> value1 >> value2) {
        if ((value1 == Uid) && (value2 == Uid)) {
          return user;
        }
      }
    }
  }
  return {};
}

//Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  string number;
  int count = 0;
  int up_time = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> number) {
      if(count == 21) {
        up_time = std::stol(number) / sysconf(_SC_CLK_TCK);
        return up_time;
      }
      count++;
    }
  }
  return up_time;
}