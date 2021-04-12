#include <iostream>

#include "wmi.h"
#include "hardware.h"

using namespace std;
using namespace benchfactor;

namespace benchfactor::hardwareinfo {
    vector<RAMInfo> get_ram_info() {
        WMIService wmi;
        vector<WMIObject> rams =  wmi.query("SELECT * FROM Win32_PhysicalMemory");

        vector<RAMInfo> ramInfos;

        for (auto ram : rams) {
            RAMInfo info;

            info.manufacturer = ram.get_string("Manufacturer");
            info.partNumber = ram.get_string("PartNumber");
            info.bankLabel = ram.get_string("BankLabel");
            info.deviceLocator = ram.get_string("DeviceLocator");
            info.capacity = ram.get_uint64("Capacity");
            info.configuredVoltage = ram.get_uint("ConfiguredVoltage");
            info.speed = ram.get_uint("Speed");

            ramInfos.push_back(info);
        }

        return ramInfos;
    }
}
