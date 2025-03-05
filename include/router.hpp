#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <regex>

using RouteHandler = std::function<std::string(const std::unordered_map<std::string, std::string>&)>;


class Router {
public:
    void get(const std::string& path, RouteHandler handler);
    void post(const std::string& path, RouteHandler handler);
    std::string handleRequest(const std::string& method, const std::string& path);
    bool has_route(const std::string& method, const std::string& path);

private:
    struct Route {
        std::regex pattern;
        RouteHandler handler;
        std::vector<std::string> paramNames;
    };

    std::unordered_map<std::string, std::vector<Route>> routes;

    std::unordered_map<std::string, std::string> extractParams(const std::string& path, const Route& route);
};

#endif
