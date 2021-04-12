#include <iostream>

#include "wmi.h"
#include "hardware.h"

using namespace std;
using namespace benchfactor;

namespace benchfactor::hardwareinfo {
    vector<CPUInfo> get_cpu_info() {
        WMIService wmi;
        vector<WMIObject> cpus =  wmi.query("SELECT * FROM Win32_Processor");

        vector<CPUInfo> cpuInfos;

        for (auto cpu : cpus) {
            CPUInfo info;

            info.manufacturer = cpu.get_string("Manufacturer");
            info.name = cpu.get_string("Name");
            info.numberOfCores = cpu.get_uint("NumberOfCores");
            info.numberOfLogicalProcessors = cpu.get_uint("NumberOfLogicalProcessors");
            info.l2CacheSize = cpu.get_uint("L2CacheSize");
            info.l3CacheSize = cpu.get_uint("L3CacheSize");
            info.maxClockSpeed = cpu.get_uint("MaxClockSpeed");

            cpuInfos.push_back(info);
        }

        return cpuInfos;
    }
}
