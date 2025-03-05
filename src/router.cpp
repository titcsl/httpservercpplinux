#include "router.hpp"
#include <iostream>

void Router::get(const std::string& path, RouteHandler handler) {
    std::string pattern = "^" + std::regex_replace(path, std::regex(R"(/:([^/]+))"), R"(/([^/]+))") + "$";
    std::regex regexPattern(pattern);

    std::vector<std::string> paramNames;
    std::smatch match;
    std::string temp = path;
    while (std::regex_search(temp, match, std::regex(R"(/:([^/]+))"))) {
        paramNames.push_back(match[1]);
        temp = match.suffix();
    }

    routes["GET"].push_back({regexPattern, handler, paramNames});
}
bool Router::has_route(const std::string& method, const std::string& path) {
    if (routes.find(method) == routes.end()) return false;

    for (const auto& route : routes[method]) {
        if (std::regex_match(path, route.pattern)) {
            return true;
        }
    }
    return false;
}

void Router::post(const std::string& path, RouteHandler handler) {
    std::string pattern = "^" + std::regex_replace(path, std::regex(R"(/:([^/]+))"), R"(/([^/]+))") + "$";
    std::regex regexPattern(pattern);

    std::vector<std::string> paramNames;
    std::smatch match;
    std::string temp = path;
    while (std::regex_search(temp, match, std::regex(R"(/:([^/]+))"))) {
        paramNames.push_back(match[1]);
        temp = match.suffix();
    }

    routes["POST"].push_back({regexPattern, handler, paramNames});
}

std::unordered_map<std::string, std::string> Router::extractParams(const std::string& path, const Route& route) {
    std::smatch match;
    std::unordered_map<std::string, std::string> params;

    if (std::regex_match(path, match, route.pattern)) {
        for (size_t i = 1; i < match.size(); ++i) {
            params[route.paramNames[i - 1]] = match[i];
        }
    }
    return params;
}

std::string Router::handleRequest(const std::string& method, const std::string& path) {
    if (routes.find(method) == routes.end()) {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoute Not Found";
    }

    for (const auto& route : routes[method]) {
        std::smatch match;
        if (std::regex_match(path, match, route.pattern)) {
            std::unordered_map<std::string, std::string> params = extractParams(path, route);
            return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + route.handler(params);
        }
    }

    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoute Not Found";
}
