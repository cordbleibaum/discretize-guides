#include <cstdlib>
#include <map>
#include <functional>
#include <string>
#include <iostream>
#include <filesystem>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../benchmarks/ram.h"

using json = nlohmann::json;
namespace pt = boost::property_tree;

const std::string settings_filename = "settings.json";

std::string graphql_call(std::string server, std::string data) {
    CURL *curl = curl_easy_init();
    if (curl) {
        std::string returnBuffer;

        struct curl_slist *headers=NULL;
        headers = curl_slist_append(headers, "charsets: utf-8");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, server);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"query\" : \"" + data + "\"}");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char *contents, size_t size, size_t nmemb, void *userp) {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &returnBuffer);
        curl_easy_perform(curl);

        std::cout << returnBuffer << "test" << std::endl;

        curl_easy_cleanup(curl);
    }
    return "";
}

std::string get_system_id() {
    std::string system_id = "";

    if (std::filesystem::exists(settings_filename)) {
        pt::ptree tree;
        pt::read_json(settings_filename, tree);
        system_id = tree.get<std::string>(settings_filename, "");
    }

    if (system_id == "") {
        graphql_call("localhost:4000", "\
            mutation addSystem {\
                createSystemID{\
                    id\
                }\
            }\
        ");
    }

    return system_id;
}

void save_system_id(std::string system_id) {
    pt::ptree tree;
    tree.add("system_id", system_id);
    pt::write_json(settings_filename, tree);
}

int main() {
    std::map<std::string, std::function<double()>> benchmarks;
    json results;

    benchmarks["ram_bandwidth_multicore"] = benchfactor::benchmarks::ram_bandwidth_multicore;

    for (const auto& [name, function] : benchmarks) {
        results[name] = function();
    }

    std::cout << results.dump(4);

    std::string system_id = get_system_id();

    // TODO

    save_system_id(system_id);
    return EXIT_SUCCESS;
}
