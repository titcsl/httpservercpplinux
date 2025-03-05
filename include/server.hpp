#ifndef SERVER_HPP
#define SERVER_HPP

#include <thread>
#include <vector>

class HTTPServer {
public:
    HTTPServer(int port);
    void start();

private:
    int server_socket;
    int port;
    std::vector<std::thread> threads;

    void handle_client(int client_socket);
};

#endif
