#include <iostream>
#include <string>

#include "ssh_client.hpp"

int main(int argc, char* argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <host> <port> <username> <password>\n";
            return 1;
        }

        const std::string host = argv[1];
        const int port = std::stoi(argv[2]);
        const std::string username = argv[3];
        const std::string password = argv[4];

        SshClient client(host, port, username, password);

        std::cout << "Connected. Type commands, or 'exit'/'quit' to stop.\n";

        std::string command;
        while (true) {
            std::cout << "> " << std::flush;

            if (!std::getline(std::cin, command)) {
                break;
            }

            if (command == "exit" || command == "quit") {
                break;
            }

            if (command.empty()) {
                continue;
            }

            const std::string output = client.execute(command);
            std::cout << output;
            if (!output.empty() && output.back() != '\n') {
                std::cout << '\n';
            }
        }

        std::cout << "Disconnected.\n";

    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}