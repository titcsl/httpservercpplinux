#include "server.hpp"
#include "router.hpp"
#include <iostream>
#include <iostream>
#include <unistd.h>       // For read(), close()
#include <sys/types.h>    // For socket types
#include <sys/socket.h>   // For socket(), bind(), listen(), accept(), send()
#include <netinet/in.h>   // For sockaddr_in
#include <cstring>        // For memset()

#include <sstream>  // For std::istringstream

std::string parse_path(const std::string& request) {
    std::istringstream requestStream(request);
    std::string method, path;
    requestStream >> method >> path;  // Extracts second word, which is the path
    return path;
}

std::string parse_method(const std::string& request) {
    std::istringstream requestStream(request);
    std::string method;
    requestStream >> method; // Extracts the first word (GET, POST, etc.)
    return method;
}


Router router;

void setupRoutes() {
    router.get("/hello", [](const std::unordered_map<std::string, std::string>& params) {
        return "Hello, World!";
    });

    router.get("/user/:id", [](const std::unordered_map<std::string, std::string>& params) {
        return "User ID: " + params.at("id");
    });

    router.post("/login", [](const std::unordered_map<std::string, std::string>& params) {
        return "Login Successful";
    });

    std::cout << "[DEBUG] Routes registered successfully!" << std::endl;
}

HTTPServer::HTTPServer(int port) {
    int server_port = port;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Binding failed");
    }

    if (listen(server_socket, 5) < 0) {
        throw std::runtime_error("Listening failed");
    }
}

void HTTPServer::start() {
    std::cout << "Server started on port " << std::endl;
    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket >= 0) {
            std::thread(&HTTPServer::handle_client, this, client_socket).detach();
        }
    }
}

void HTTPServer::handle_client(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));

    std::string request(buffer);
    std::string method = parse_method(request);
    std::string path = parse_path(request);

    std::cout << "[DEBUG] Received Request: " << method << " " << path << std::endl;

    std::string response;
    if (router.has_route(method, path)) {
        response = router.handleRequest(method, path);
    } else {
        std::cout << "[DEBUG] 404 Not Found: " << method << " " << path << std::endl;
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoute Not Found";
    }

    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}

