#pragma once

#include <string>
#include <vector>

namespace benchfactor::hardwareinfo {
    struct CPUInfo {
        std::string manufacturer;
        std::string name;
        unsigned int numberOfCores;
        unsigned int numberOfLogicalProcessors;
        unsigned int l2CacheSize;
        unsigned int l3CacheSize;
        unsigned int maxClockSpeed;
    };

    std::vector<CPUInfo> get_cpu_info();

    struct MainboardInfo {
        std::string manufacturer;
        std::string product;
    };

    MainboardInfo get_mainboard_info();

    struct DriveInfo {
        std::string model;
        std::string firmwareRevision;
        uint64_t size;
    };

    std::vector<DriveInfo> get_drive_info();

    struct RAMInfo {
        std::string manufacturer;
        std::string partNumber;
        std::string bankLabel;
        std::string deviceLocator;
        uint64_t capacity;
        uint32_t speed;
        uint32_t configuredVoltage;
    };

    std::vector<RAMInfo> get_ram_info();

    struct GPUInfo {
        std::string videoProcessor;
        std::string adapterCompatibility;
        std::string name;
        std::string driverVersion;
        uint64_t adapterRAM;
        uint32_t currentVerticalResolution;
        uint32_t currentHorizontalResolution;
        uint32_t currentBitsPerPixel;
        uint32_t currentRefreshRate;
    };

    std::vector<GPUInfo> get_gpu_info();
}
