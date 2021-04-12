#include <iostream>

#include "wmi.h"
#include "hardware.h"

using namespace std;
using namespace benchfactor;

namespace benchfactor::hardwareinfo {
    MainboardInfo get_mainboard_info() {
        WMIService wmi;
        vector<WMIObject> mainboards =  wmi.query("SELECT * FROM Win32_BaseBoard");
        WMIObject mainboard = mainboards[0];

        MainboardInfo info;

        info.manufacturer = mainboard.get_string("Manufacturer");
        info.product = mainboard.get_string("Product");

        return info;
    }
}
