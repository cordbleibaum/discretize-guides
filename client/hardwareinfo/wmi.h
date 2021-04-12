#include <vector>
#include <string>
#include <memory>

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

namespace benchfactor
{
    class WMIObject {
    public:
        WMIObject(IWbemClassObject *classObject);
        std::string get_string(std::string key);
        unsigned int get_uint(std::string key);
        uint64_t get_uint64(std::string key);
    private:
        std::shared_ptr<IWbemClassObject> classObject;
    };

    class WMIService {
    public:
        WMIService();
        std::vector<WMIObject> query(std::string wql);
        ~WMIService();
    private:
        IWbemLocator *wmiLocator = NULL;
        IWbemServices *wmiService = NULL;
    };
}