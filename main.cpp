#include <array>
#include <libssh2.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

class Libssh2Guard {
public:
    Libssh2Guard() {
        if (libssh2_init(0) != 0) {
            throw std::runtime_error("libssh2_init failed");
        }
    }

    ~Libssh2Guard() { libssh2_exit(); }

    Libssh2Guard(const Libssh2Guard &) = delete;
    Libssh2Guard &operator=(const Libssh2Guard &) = delete;
};

struct SocketDeleter {
    void operator()(int *socketFd) const noexcept {
        if (socketFd) {
            if (*socketFd >= 0) {
                ::close(*socketFd);
            }
            delete socketFd;
        }
    }
};

struct SessionDeleter {
    void operator()(LIBSSH2_SESSION *session) const noexcept {
        if (session) {
            libssh2_session_disconnect(session, "Normal shutdown");
            libssh2_session_free(session);
        }
    }
};

struct ChannelDeleter {
    void operator()(LIBSSH2_CHANNEL *channel) const noexcept {
        if (channel) {
            libssh2_channel_close(channel);
            libssh2_channel_free(channel);
        }
    }
};

class SshClient {
public:
    SshClient(std::string host, int port, std::string username, std::string password) :
        m_host(std::move(host)), m_port(port), m_username(std::move(username)), m_password(std::move(password)),
        m_libssh2Guard(std::make_unique<Libssh2Guard>()) {
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

    std::string execute(std::string_view command) {
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

private:
    static std::unique_ptr<int, SocketDeleter> connectTcp(std::string_view host, int port) {
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

    std::string m_host;
    int m_port;
    std::string m_username;
    std::string m_password;

    std::unique_ptr<Libssh2Guard> m_libssh2Guard;
    std::unique_ptr<int, SocketDeleter> m_socket;
    std::unique_ptr<LIBSSH2_SESSION, SessionDeleter> m_session;
};

int main() {
    try {
        SshClient client("172.17.135.124", 22, "user", "Adid@$");

        std::cout << client.execute("whoami");
        std::cout << client.execute("ls -l /tmp");
    } catch (const std::exception &exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
