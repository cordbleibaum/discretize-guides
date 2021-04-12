#include <iostream>

#include "wmi.h"
#include "hardware.h"

using namespace std;
using namespace benchfactor;

namespace benchfactor::hardwareinfo {
    vector<GPUInfo> get_gpu_info() {
        WMIService wmi;
        vector<WMIObject> gpus =  wmi.query("SELECT * FROM Win32_VideoController");

        vector<GPUInfo> gpuInfos;

        for (auto gpu : gpus) {
            GPUInfo info;

            info.videoProcessor = gpu.get_string("VideoProcessor");
            info.name = gpu.get_string("Name");
            info.adapterCompatibility = gpu.get_string("AdapterCompatibility");
            info.driverVersion = gpu.get_string("DriverVersion");
            info.adapterRAM = gpu.get_uint("AdapterRAM");
            info.currentVerticalResolution = gpu.get_uint("CurrentVerticalResolution");
            info.currentHorizontalResolution = gpu.get_uint("CurrentHorizontalResolution");
            info.currentBitsPerPixel = gpu.get_uint("CurrentBitsPerPixel");
            info.currentRefreshRate = gpu.get_uint("CurrentRefreshRate");

            gpuInfos.push_back(info);
        }

        return gpuInfos;
    }
}
