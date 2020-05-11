#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  long Active_Jiffies = LinuxParser::ActiveJiffies();
  long Total_Jiffies = LinuxParser::Jiffies();
  return float(Active_Jiffies) / float(Total_Jiffies);
}