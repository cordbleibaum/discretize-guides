#include "wmi.h"

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>

using namespace std;

namespace benchfactor
{
    WMIService::WMIService() {
        HRESULT hres;

        hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
            return;
        }

        hres =  CoInitializeSecurity(
            NULL, 
            -1,                          // COM authentication
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities 
            NULL                         // Reserved
        );

        if (FAILED(hres))
        {
            cout << "Failed to initialize security. Error code = 0x" << hex << hres << endl;
            CoUninitialize();
        }

        hres = CoCreateInstance(
            CLSID_WbemLocator,             
            0, 
            CLSCTX_INPROC_SERVER, 
            IID_IWbemLocator, (LPVOID *) &wmiLocator);
    
        if (FAILED(hres))
        {
            cout << "Failed to create IWbemLocator object." << " Err code = 0x" << hex << hres << endl;
            CoUninitialize();
        }
    
        hres = wmiLocator->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
            NULL,                    // User name. NULL = current user
            NULL,                    // User password. NULL = current
            0,                       // Locale. NULL indicates current
            NULL,                    // Security flags.
            0,                       // Authority (for example, Kerberos)
            0,                       // Context object 
            &wmiService              // pointer to IWbemServices proxy
        );
        
        if (FAILED(hres))
        {
            cout << "Could not connect. Error code = 0x" << hex << hres << endl;
            wmiLocator->Release();     
            CoUninitialize();
        }

        
        hres = CoSetProxyBlanket(
        wmiService,                  // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
        );

        if (FAILED(hres))
        {
            cout << "Could not set proxy blanket. Error code = 0x" << hex << hres << endl;
            wmiService->Release();
            wmiLocator->Release();     
            CoUninitialize();
        }
    }

    std::vector<WMIObject> WMIService::query(std::string wql) {
        std::vector<WMIObject> objects;

        IEnumWbemClassObject* enumerator = NULL;
        HRESULT hres = wmiService->ExecQuery(
            bstr_t("WQL"), 
            bstr_t(wql.c_str()),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
            NULL,
            &enumerator);
        
        if (FAILED(hres))
        {
            cout << "Query failed: " << wql << " Error code = 0x" << hex << hres << endl;
            return objects;
        }
    
        while (enumerator)
        {
            IWbemClassObject *classObject = NULL;
            ULONG returnValue = 0;
            HRESULT hr = enumerator->Next(WBEM_INFINITE, 1, &classObject, &returnValue);

            if(returnValue == 0)
            {
                break;
            }

            objects.emplace_back(classObject);
        }

        enumerator->Release();

        return objects;
    }

    WMIService::~WMIService() 
    {
        wmiService->Release();
        wmiLocator->Release();
        CoUninitialize();
    }

    WMIObject::WMIObject(IWbemClassObject *classObject) 
        : classObject(classObject, [](IWbemClassObject* object) {
            object->Release();
        })
    {}

    string WMIObject::get_string(string key) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring key_wide = converter.from_bytes(key);

        VARIANT vtProp;
        HRESULT hr = classObject->Get(key_wide.c_str(), 0, &vtProp, 0, 0);
        wstring result(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
        string narrow_result = converter.to_bytes(result);

        VariantClear(&vtProp);

        return narrow_result;
    }

    unsigned int WMIObject::get_uint(string key) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring key_wide = converter.from_bytes(key);

        VARIANT vtProp;
        HRESULT hr = classObject->Get(key_wide.c_str(), 0, &vtProp, 0, 0);

        unsigned int result = vtProp.uintVal;

        VariantClear(&vtProp);

        return result;
    }


    uint64_t WMIObject::get_uint64(string key) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring key_wide = converter.from_bytes(key);

        VARIANT vtProp;
        HRESULT hr = classObject->Get(key_wide.c_str(), 0, &vtProp, 0, 0);
        wstring wsresult(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
        string narrow_result = converter.to_bytes(wsresult);

        uint64_t result = stoull(narrow_result);

        VariantClear(&vtProp);

        return result;
    }
}
