#include <iostream>

#include "ssh_client.hpp"

int main() {
    try {
        SshClient client("172.17.135.116", 22, "user", "Adid@$");

        std::cout << client.execute("whoami");
        std::cout << client.execute("ls -l /tmp");
        std::cout << client.execute("journalctl --since=-20h");
    } catch (const std::exception &exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
