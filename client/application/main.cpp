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
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "../benchmarks/ram.h"

using json = nlohmann::json;
namespace pt = boost::property_tree;
namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

const std::string settings_filename = "settings.json";

json graphql_call(std::string_view host, const std::string_view target, int port, const std::string& data) {
    try {
        net::io_context io_context;
        tcp::resolver resolver(io_context);
        beast::tcp_stream stream(io_context);

        const auto dns_results = resolver.resolve(host, std::to_string(port));
        stream.connect(dns_results);

        std::string body = "{\"query\" : \"" + data + "\"}";

        http::request<http::string_body> request{http::verb::post, target, 11};
        request.set(http::field::host, host);
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.set(http::field::content_type, "application/json");
        request.set(http::field::content_length, std::to_string(body.length()));
        request.body() = body;
        http::write(stream, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(stream, buffer, response);

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        return json::parse(response.body());
    }  catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return "";
}

std::string get_system_id() {
    std::string system_id = "";

    if (std::filesystem::exists(settings_filename)) {
        pt::ptree tree;
        pt::read_json(settings_filename, tree);
        system_id = tree.get<std::string>("system_id", "");
    }

    if (system_id == "") {
        auto result = graphql_call("localhost", "/", 4000, "\
            mutation addSystem {\
                createSystemID{\
                    id\
                }\
            }\
        ");
        system_id = result["data"]["createSystemID"]["id"];
        std::cout << "Created ID: " << system_id << std::endl;
    }

    return system_id;
}

void save_system_id(const std::string_view system_id) {
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

    const std::string system_id = get_system_id();

    // TODO

    save_system_id(system_id);
    return EXIT_SUCCESS;
}
