#include "server.hpp"

int main() {
    HTTPServer server(8080);
    server.start();
    return 0;
}
