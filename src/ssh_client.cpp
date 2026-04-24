#include "ssh_client.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <array>
#include <cstring>
#include <string>

Libssh2Guard::Libssh2Guard() {
    if (libssh2_init(0) != 0) {
        throw std::runtime_error("libssh2_init failed");
    }
}
Libssh2Guard::~Libssh2Guard() { libssh2_exit(); }

SshClient::SshClient(std::string_view host, int port, std::string_view username, std::string_view password)
    : m_host{host}, m_username{username}, m_password{password}, m_libssh2Guard{std::make_unique<Libssh2Guard>()}, m_port{port} {
    m_socket = connectTcp(m_host, m_port);
    if (!m_socket || *m_socket < 0) {
        throw std::runtime_error("TCP connection failed: " + std::string(std::strerror(errno)));
    }

    LIBSSH2_SESSION *rawSession = libssh2_session_init();
    if (!rawSession) {
        throw std::runtime_error("libssh2_session_init failed");
    }

    m_session.reset(rawSession);

    if (libssh2_session_handshake(m_session.get(), *m_socket) != 0) {
        throw std::runtime_error("SSH handshake failed");
    }

    if (libssh2_userauth_password(m_session.get(), m_username.c_str(), m_password.c_str()) != 0) {
        throw std::runtime_error("Authentication failed");
    }
}
std::string SshClient::execute(std::string_view command) {
    LIBSSH2_CHANNEL *rawChannel = libssh2_channel_open_session(m_session.get());
    if (!rawChannel) {
        throw std::runtime_error("Could not open SSH channel");
    }

    std::unique_ptr<LIBSSH2_CHANNEL, ChannelDeleter> channel(rawChannel);

    std::string commandString(command);
    if (libssh2_channel_exec(channel.get(), commandString.c_str()) != 0) {
        throw std::runtime_error("Could not execute command");
    }

    std::string output;
    std::array<char, 4096> buffer;

    for (;;) {
        ssize_t readBytes = libssh2_channel_read(channel.get(), buffer.data(), buffer.size());
        if (readBytes > 0) {
            output.append(buffer.data(), static_cast<size_t>(readBytes));
        } else if (readBytes == LIBSSH2_ERROR_EAGAIN) {
            continue;
        } else {
            break;
        }
    }

    return output;
}
std::unique_ptr<int, SocketDeleter> SshClient::connectTcp(std::string_view host, int port) {
    addrinfo hints{};
    addrinfo *result = nullptr;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    const std::string hostString(host);
    const std::string portString = std::to_string(port);

    if (getaddrinfo(hostString.c_str(), portString.c_str(), &hints, &result) != 0) {
        return std::unique_ptr<int, SocketDeleter>(new int(-1));
    }

    int socketFd = -1;
    for (addrinfo *current = result; current != nullptr; current = current->ai_next) {
        socketFd = ::socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (socketFd == -1) {
            continue;
        }

        if (::connect(socketFd, current->ai_addr, current->ai_addrlen) == 0) {
            break;
        }

        ::close(socketFd);
        socketFd = -1;
    }

    freeaddrinfo(result);
    return std::unique_ptr<int, SocketDeleter>(new int(socketFd));
}