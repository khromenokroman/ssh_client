#include <boost/program_options.hpp>
#include <iostream>
#include <string>

#include "../include/ssh_client.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    try {
        std::string host;
        std::string port_string;
        std::string username;
        std::string password;

        po::options_description options("Allowed options");
        options.add_options()("help,h", "show help message")("host", po::value<std::string>(&host)->required(), "ssh host")(
            "port", po::value<std::string>(&port_string)->required(), "ssh port")(
            "user", po::value<std::string>(&username)->required(), "ssh username")("pass", po::value<std::string>(&password)->required(),
                                                                                   "ssh password");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, options), vm);

        if (vm.count("help")) {
            std::cout << options << '\n';
            return EXIT_SUCCESS;
        }

        po::notify(vm);

        const int port = std::stoi(port_string);

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

            std::string const output = client.execute(command);
            std::cout << output;
            if (!output.empty() && output.back() != '\n') {
                std::cout << '\n';
            }
        }

        std::cout << "Disconnected.\n";
    } catch (std::exception const& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}