#include <iostream>
#include <csignal>
#include "server.h"

int main(int argc, char **argv) {
  // check for valid arguments
  if (argc != 2) {
    std::cerr << "Usage: server_main <port>\n";
    return 1;
  }

  // get port from user input
  int port = std::stoi(argv[1]);

  // ignore SIGPIPE: when the server sends data to the receive client,
  // it may find that the connection has been terminated (e.g., if the
  // receive client exited)
  signal(SIGPIPE, SIG_IGN);

  // init server and try to listen on given port, if fails, exit with error status 1
  Server server(port);
  if (!server.listen()) {
    std::cerr << "Could not listen on port " << port << "\n";
    return 1;
  }

  // server communicates with clients
  server.handle_client_requests();
}
