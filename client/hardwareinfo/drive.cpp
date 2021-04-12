#include <iostream>

#include "wmi.h"
#include "hardware.h"

using namespace std;
using namespace benchfactor;

namespace benchfactor::hardwareinfo {
    vector<DriveInfo> get_drive_info() {
        WMIService wmi;
        vector<WMIObject> drives =  wmi.query("SELECT * FROM Win32_DiskDrive");

        vector<DriveInfo> driveInfos;

        for (auto drive : drives) {
            DriveInfo info;

            info.model = drive.get_string("Model");
            info.firmwareRevision = drive.get_string("FirmwareRevision");
            info.size = drive.get_uint64("Size");
            cout << info.size << endl;

            driveInfos.push_back(info);
        }

        return driveInfos;
    }
}
